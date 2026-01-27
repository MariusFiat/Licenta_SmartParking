Tot ce rulez trebuie sa treaca prin venv!!

Cum rulez push_data.py pentru test:
/Users/mariusfiat/.venv_licenta/bin/python3 push_data.py

Pentru partea de detect_plate:
source .venv/bin/activate && pip install opencv-python && python main_run.py = activez venv si instalez opencv
source .venv/bin/activate && pip install torch && python main_run.py = instalez torch
source .venv/bin/activate && pip install pillow && python main_run.py = instalez PIL
source .venv/bin/activate && pip install torchvision && python main_run.py
source .venv/bin/activate && pip install ultralytics && python main_run.py
source .venv/bin/activate && pip install pytorch-lightning timm && python main_run.py
source .venv/bin/activate && pip install nltk && python main_run.py
pip install timm pillow pyyaml

Idei aparute pe parcurs:
Pentru macheta:
    - Structura din carton mucava
    - Pe structura adaug bucatele de hartie/ziar cu aracet pentru duritate
    - Adaug alb ca si strat de baza si apoi culori

Pentru software:
    - Vreau sa se aprinda becurile in functie de locul asociat masinii pentru a lumina parcarea pana la acest loc, asta o sa fac cu grafuri/arbori oare? (Harta luminilor)

Pentru Raspberry PI Pico
    - Sa folosesc FreeRTOS pentru a avea un efect de paralelism pe task-urile pe care le are de executat.