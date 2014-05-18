Install
=======

Python frontend
---------------

```bash
pip install -r requirements.txt
./main.py --debug
````

Runner thing
------------

You'll need libzmq, libmsgpack and kyoto cabinet. Make sure that the uri you
use to run the jobrunner is the same as the one in `settings.py`.

```bash
sudo apt-get instal libzmq-dev libmsgpack-dev
make
./jobrunner 'tcp://*:1979'
````
