cd ./.venv/bin
echo "Activating virtual environment..."
source activate
cd ../../
echo "Running test_connection.py to verify setup..."
python3 test_connection.py