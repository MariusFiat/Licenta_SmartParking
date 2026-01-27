Scriptul de detectie va rula constant, fiind deschis la initializarea sistemului dupa care asteapta comenzi de la interfata seriala.

Test pentru a simula comenzile venite de la raspberry:
socat -d -d pty,raw,echo=0 pty,raw,echo=0 -> Creaza doua terminale conectate intre ele
