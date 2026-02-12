source .venv/bin/activate

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Lansare sistem de procesare..."

# 1. Deschide un terminal nou și rulează Consumatorul
osascript -e "tell application \"Terminal\" to do script \"cd '$DIR' && python3 main_test_pipe.py\""

# 2. Așteaptă o secundă pentru a asigura crearea pipe-ului de către consumator (opțional)
sleep 1

# 3. Deschide un alt terminal și rulează Producătorul
source .venv/bin/activate
# osascript -e "tell application \"Terminal\" to do script \"cd '$DIR' && python3 run_model.py\""
python3 run_model.py &

echo "Ambele procese au fost lansate în ferestre separate."