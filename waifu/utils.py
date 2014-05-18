from waifu.settings import ALLOWED_FILE_EXTENSIONS, RUNNER_URI
import random, string, msgpack, zmq

def send_to_runner(msg):
    context = zmq.Context.instance()
    sock = context.socket(zmq.REQ)
    sock.SNDTIMEO = 2000
    sock.RCVTIMEO = 2000

    sock.connect(RUNNER_URI)
    sock.send(msg, copy=False)

    try:
        response = sock.recv()
    except zmq.ZMQError:
        return False

    return response

def schedule_new_query(filepath):
    msg = {
        'cmd': 'query',
        'filepath': filepath
    }
    packed = msgpack.packb(msg)
    resp = send_to_runner(packed)

    return resp

def allowed_file(filename):
    return '.' in filename and filename.lower().rsplit('.', 1)[1] in ALLOWED_FILE_EXTENSIONS

def random_csrf():
    myrg = random.SystemRandom()
    length = 32
    # If you want non-English characters, remove the [0:52]
    alphabet = string.letters[0:52] + string.digits
    pw = str().join(myrg.choice(alphabet) for _ in range(length))
    return pw
