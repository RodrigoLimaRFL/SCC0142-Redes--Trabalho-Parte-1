// fragmentacao.cpp
#include "fragmentacao.hpp"

template <size_t N>
bitset<N> vectorToBitset(const std::vector<uint8_t>& vec) {
    /*
    * Converte um vetor de bytes (uint8_t) em um bitset de tamanho N.
    *
    * Params:
    * - vec: Vetor de bytes a ser convertido.
    *
    * Returns:
    * - Um bitset de tamanho N representando os bits do vetor.
    */
    std::bitset<N> bits;

    size_t bitIndex = 0;
    for (size_t byteIndex = 0; byteIndex < vec.size() && bitIndex < N; ++byteIndex) {
        uint8_t byte = vec[byteIndex];
        for (int i = 0; i < 8 && bitIndex < N; ++i) {
            bool bit = (byte >> i) & 1;
            bits[bitIndex++] = bit;
        }
    }

    return bits;
}

template <size_t N>
bitset<N> uInt32ToBitset(uint32_t value) {
    /*
    * Converte um valor uint32_t em um bitset de tamanho N.
    * Params:
    * - value: Valor a ser convertido.
    * Returns:
    * - Um bitset de tamanho N representando os bits do valor.
     */
    bitset<N> bits;
    for (size_t i = 0; i < N && value > 0; ++i) {
        bits[i] = (value & 1);
        value >>= 1;
    }
    return bits;
}

vector<vector<uint8_t>> fragmentarDados(vector<uint8_t> dados, Session& session)
{
    /**
     * Fragmenta os dados em pacotes de tamanho máximo definido.
     *
     * Params:
     * - dados: Dados a serem fragmentados.
     * - session: Sessão atual para obter o tamanho da janela e outros parâmetros.
     *
     * Returns:
     * - Um vetor de vetores de bytes, onde cada vetor representa um pacote fragmentado.
     */
    int numBytes = dados.size();

    int windowSize = session.getWindow();
    // The problem statement says "window: Window Size, quantidade de espaços para receber pacotes no buffer de leitura após o último ack - 16 bits"
    // And "data: Data, máximo de 1440 bytes."
    // The total packet size cannot exceed 1472 bytes (including header). Header is 32 bytes.
    // 1472 - 32 = 1440.
    // The window size here is interpreted as the maximum amount of *data* bytes we can send in a fragment,
    // constrained by both the protocol's max data size and the receiver's window.
    int tamanhoFragmentacao = std::min(TAMANHO_MAXIMO_DADOS, windowSize);

    if (tamanhoFragmentacao <= 0) {
        std::cerr << "Erro: Tamanho de fragmentação inválido (<= 0). Verifique a janela ou TAMANHO_MAXIMO_DADOS." << std::endl;
        return {}; // Return empty if fragmentation size is invalid
    }

    int numPacotes = (numBytes + tamanhoFragmentacao - 1) / tamanhoFragmentacao; // Ceiling division
    vector<vector<uint8_t>> dadosParaPacotes;

    for(int i = 0; i < numPacotes; i++)
    {
        vector<uint8_t> pacote;
        int inicio = i * tamanhoFragmentacao;
        int fim = inicio + tamanhoFragmentacao;

        if(fim > numBytes) {
            fim = numBytes; // Ajusta o fim para não ultrapassar o tamanho dos dados
        }

        for(int j = inicio; j < fim; j++) {
            pacote.push_back(dados[j]);
        }

        dadosParaPacotes.push_back(pacote);
    }

    return dadosParaPacotes;
}

uint8_t fragmentId = 0; // ID do fragmento - moved outside to persist across calls

// Helper function to send a single packet and handle its response
PacoteSlow sendPacketAndWaitForAck(PacoteSlow packet, const std::string& packetType, Session& session) {
    imprimirPacote(packet, packetType);
    PacoteSlow response = sendReceive(packet);
    imprimirPacote(response, "Resposta do " + packetType);

    // Update session sequence number based on ACK from central if it's a data packet
    // The problem description states:
    // - Central's Ack: "seqnum: próximo número de sequência", "acknum: número de sequência do último pacote recebido (o de dados)"
    // - Peripheral's Data: "seqnum: próximo número de sequência", "acknum: número de sequência do último pacote recebido"
    // This means central's `acknum` is the peripheral's `seqNum` it's acknowledging.
    // And central's `seqNum` is its own next sequence number.
    // So, for peripheral, `session.setAckNum` should be updated with central's `seqNum`,
    // and `session.setSeqNum` should be updated with central's `ackNum` (which is the peripheral's acknowledged `seqNum`).

    // If the response is an ACK for data (flags.test(2) is ACK), update session's sequence and ack numbers.
    // The central's response's 'seqNum' becomes the peripheral's new 'ackNum' (last received sequence from central).
    // The central's response's 'ackNum' becomes the peripheral's new 'seqNum' (the highest sequence number the central acknowledged).
    if (response.getFlags().test(2)) { // If ACK flag is set in the response
        session.setAckNum(response.getSeqNum());
        session.setSeqNum(response.getAckNum());
    } else {
        // For other types of responses (e.g., Setup after Connect, or a rejected Revive),
        // we update all session values to reflect the central's state.
        session.setValues(response);
    }

    return response;
}

bool fragmentarEEnviarDados(
    vector<uint8_t> uuid,
    uint32_t sttl,
    uint32_t ultimoSeqNum, // This parameter is currently not used directly in packet creation inside loop, using session's state
    uint32_t ultimoAckNum, // This parameter is currently not used directly in packet creation inside loop, using session's state
    uint16_t window,
    vector<uint8_t> dados,
    TiposDeEnvio tipoEnvio,
    Session& session
) {
    vector<vector<uint8_t>> pacotes = fragmentarDados(dados, session);

    bitset<128> uuidBits = vectorToBitset<128>(uuid);
    bitset<27> sttlBits = uInt32ToBitset<27>(sttl);

    // Use a unique fragment ID for this entire set of fragments
    // "fid: Fragment ID, pacotes com o mesmo fid devem ser agrupados sequancialmente - 8 bits"
    // "fo: Fragment Offset, ordem de um pacote dentro de um Fragment ID - 8 bits"
    uint8_t currentFragmentId = fragmentId++; // Increment after each full set of fragments sent

    for(size_t i = 0; i < pacotes.size(); i++) {
        // "More Bits: este pacote é parte de um pacote maior que foi fragmentado, e mais fragmentos chegarão"
        bool maisDados = (i < pacotes.size() - 1);
        if (session.getSTTL() <= 0) {
            cout << "Sessão expirada. Não é possível enviar pacotes." << endl;
            return false; // Se o STTL for 0, não envia pacotes
        }

        PacoteSlow pacote = setPacketToSend(
            uuidBits,
            sttlBits,
            session.getSeqNum(), // Use current session seq num as the base for the next packet
            session.getAckNum(), // Use current session ack num
            window,
            maisDados,
            pacotes[i]
        );
        pacote.setFid(currentFragmentId);
        pacote.setFo(i);

        if(tipoEnvio == TiposDeEnvio::REVIVE && i == 0) { // Only the first packet of a revive flow sets the revive flag
            bitset<5> flags = pacote.getFlags();
            flags.set(1, 1); // Set Revive flag
            pacote.setFlags(flags);

            PacoteSlow pacoteResposta = sendPacketAndWaitForAck(pacote, "Pacote de Revive", session);

            bitset<5> flagsResposta = pacoteResposta.getFlags();

            // "Accept/Reject: este pacote aceita ou rejeita uma conexão (0-way ou 3-way)"
            if(flagsResposta.test(3)) { // Check Accept/Reject flag
                cout << "Servidor aceitou o revive." << endl;
                session.setValues(pacoteResposta); // Update session values from accepted revive response
            } else {
                cout << "Servidor rejeitou o revive." << endl;
                return false;
            }
        } else {
            // For data packets, implement re-transmission with a retry mechanism.
            // The protocol states: "Enviar dados (começando uma 0-way connection ou não) e receber ack (se não receber, reenviar dados)"
            int retryCount = 0;
            const int MAX_RETRIES = 3; // Example: retry up to 3 times
            bool acknowledged = false;

            while (retryCount < MAX_RETRIES && !acknowledged) {
                PacoteSlow pacoteResposta = sendPacketAndWaitForAck(pacote, "Pacote de Dados", session);

                // Check if the response is an ACK and its ackNum acknowledges our sent packet.
                // The central's ACK will have its `acknum` equal to the `seqNum` of the peripheral's packet it's acknowledging.
                if (pacoteResposta.getFlags().test(2) && pacoteResposta.getAckNum() == pacote.getSeqNum()) { // ACK flag
                    acknowledged = true;
                    // session.setSeqNum and session.setAckNum are updated inside sendPacketAndWaitForAck
                } else {
                    std::cout << "No valid ACK received for packet with SeqNum: " << pacote.getSeqNum() << ". Retrying... (" << retryCount + 1 << "/" << MAX_RETRIES << ")" << std::endl;
                    retryCount++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait before retrying
                }
            }

            if (!acknowledged) {
                std::cout << "Failed to receive ACK after " << MAX_RETRIES << " retries. Aborting data send." << std::endl;
                return false;
            }
        }
    }
    return true; // Returns true if all packets were sent and acknowledged (or revive accepted)
}