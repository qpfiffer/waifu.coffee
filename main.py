#!/usr/bin/env python2
from flask import abort, Flask, g, request, session

from waifu.routes import app as routes
import sys, getopt, random, string, json, time

app = Flask('waifu')
app.register_blueprint(routes)
#app.register_blueprint(conprocs)

@app.before_request
def setup_db():
    if request.method == "POST":
        token = session.get('_csrf_token', None)
        in_form = token == request.form.get('_csrf_token')
        try:
            stuff = json.loads(request.data).get("_csrf_token")
            in_data = stuff == token
        except Exception as e:
            print e
            in_data = False
        if not token or not (in_form or in_data):
            abort(403)

def generate_csrf_token():
    if '_csrf_token' not in session:
        session['_csrf_token'] = random_csrf()
    return session['_csrf_token']

app.jinja_env.globals['csrf_token'] = generate_csrf_token

def main(argv):
    debug = False
    port = 5000

    try:
        opts, args = getopt.getopt(argv,"dpa",["debug", "port="])
    except getopt.GetoptError:
        print "Specifiy some options"
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("--debug"):
            debug = True
        elif opt in ("--port"):
            port = int(arg)

    app.run(debug=debug, port=port)

if __name__ == '__main__':
    main(sys.argv[1:])
