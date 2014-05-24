from flask import (g, request, current_app, Blueprint, render_template,
                   redirect, url_for, session, abort, Response)
from werkzeug.utils import secure_filename

from waifu.settings import ALLOWED_FILE_EXTENSIONS, UPLOAD_DESTINATION
from waifu.utils import allowed_file, schedule_new_query

import time, calendar, os, urllib2

app = Blueprint('waifu', __name__, template_folder='templates')

@app.route("/", methods=['GET', 'POST'])
def root():
    error = None
    path = None
    if request.method == 'POST':
        #TODO: Refactor this out
        if len(request.files) != 0:
            ufile = request.files['file']
            if ufile and allowed_file(ufile.filename):
                filename = secure_filename(ufile.filename)
                path = os.path.join(current_app.config['UPLOAD_FOLDER'], filename)
                ufile.save(path)
            else:
                error = "Filetype not supported, we currently only allow {}".format(
                ", ".join(ALLOWED_FILE_EXTENSIONS))
        elif request.form['url'] is not None:
            url = request.form['url']
            try:
                print "RETRIEVEING FILE"
                resp = urllib2.urlopen(url)
                print "FILE RETRIEVED"
                if resp.code == 200:
                    filename_bestguess = url.rsplit("/")[-1]
                    filename = secure_filename(filename_bestguess)

                    path = os.path.join(current_app.config['UPLOAD_FOLDER'], filename)
                    print "WRITING FILE"
                    output = open(path,'wb')
                    output.write(resp.read())
                    output.close()
                    print "FILE WRITTEN"
            except Exception as e:
                error = "URL provided is invalid. Images must be one of the "\
                        "following filetypes: {}".format(
                        ", ".join(ALLOWED_FILE_EXTENSIONS))
        if error is None:
            query_key = schedule_new_query(path)
            if query_key:
                return redirect(url_for('waifu.results', results_id=query_key))
            else:
                error = "Could not schedule processing job. Please try again later."

    return render_template("index.html", error=error)

@app.route("/search/<results_id>", methods=['GET'])
def results(results_id):
    return render_template("results.html")
