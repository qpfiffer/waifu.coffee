from flask import (g, request, current_app, Blueprint, render_template,
                   redirect, url_for, session, abort, Response)
import json, time, calendar

app = Blueprint('waifu', __name__, template_folder='templates')

@app.route("/", methods=['GET', 'POST'])
def root():
    if request.method == 'POST':
        import ipdb; ipdb.set_trace()
        "asdf"
    return render_template("index.html")
