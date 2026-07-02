Build pentru aplicatiile de pe RP2040.
1. Se merge la calea ./RPI_Pico
2. Se conecteaza PICO-ul la calculator
2. Se ruleaza comanda de build "make build_rp"
3. Se ruleaza comanda "make load_u2f" pentru a incarca binarele in memoria flash a microcontroler-ului

Se pornesc firele de executie si detectia:
1. Se navigheaza la calea ./Car_plate_detection
2. Se ruleaza comanda "python3 main_run.py"