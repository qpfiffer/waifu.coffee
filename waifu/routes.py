from flask import (g, request, current_app, Blueprint, render_template,
                   redirect, url_for, session, abort, Response)
from waifu.local_settings import allowed_file_extensions
from waifu.utils import allowed_file
import json, time, calendar

app = Blueprint('waifu', __name__, template_folder='templates')

@app.route("/", methods=['GET', 'POST'])
def root():
    error = None
    if request.method == 'POST':
        ufile = request.files['file']
        if ufile and allowed_file(ufile.filename):
            filename = secure_filename(ufile.filename)
            ufile.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))

            return redirect(url_for('results',
                                    filename=filename))
        else:
            error = "Filetype not supported, we currently only allow {}".format(
            ", ".join(allowed_file_extensions))
    return render_template("index.html", error=error)

@app.route("/<results_id>", methods=['GET'])
def results(results_id):
    return render_template("results.html")
