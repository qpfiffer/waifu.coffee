from waifu.settings import allowed_file_extensions, runner_uri
import random, string, msgpack, zmq

def send_to_runner(msg):
    context = zmq.Context.instance()
    sock = context.socket(zmq.REQ)
    sock.bind(runner_uri)

def schedule_new_query(filepath):
    msg = {
        'cmd': 'query',
        'filepath': filepath
    }
    packed = msgpack.packb(msg)

    return False

def allowed_file(filename):
    return '.' in filename and filename.lower().rsplit('.', 1)[1] in allowed_file_extensions

def random_csrf():
    myrg = random.SystemRandom()
    length = 32
    # If you want non-English characters, remove the [0:52]
    alphabet = string.letters[0:52] + string.digits
    pw = str().join(myrg.choice(alphabet) for _ in range(length))
    return pw
