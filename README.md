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

You'll need libzmq, libmsgpack, kyoto cabinet and some other stuff. Make sure
that the URI you use to run the jobrunner is the same as the one in `settings.py`.

```bash
sudo apt-get instal libzmq-dev libmsgpack-dev libpng++-dev
make
./jobrunner 'tcp://*:1979'
````
