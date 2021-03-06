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
        raw_response = sock.recv()
        response = msgpack.unpackb(raw_response)
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

    if resp == False or resp[0] == False:
        return False

    return resp[1]['query_key']

def allowed_file(filename):
    return '.' in filename and filename.lower().rsplit('.', 1)[1] in ALLOWED_FILE_EXTENSIONS

def random_csrf():
    myrg = random.SystemRandom()
    length = 32
    # If you want non-English characters, remove the [0:52]
    alphabet = string.letters[0:52] + string.digits
    pw = str().join(myrg.choice(alphabet) for _ in range(length))
    return pw
