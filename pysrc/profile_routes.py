from flask import Blueprint, render_template, redirect, url_for, request, flash
from flask_login import login_required, current_user
from werkzeug.utils import secure_filename
import os
from pysrc.ext import db

profile_bp = Blueprint("profile", __name__, template_folder="templates")

UPLOAD_FOLDER = "static/images"


@profile_bp.route("/profile", methods=["GET", "POST"])
@login_required
def profile():
    if request.method == "POST":
        nickname = request.form.get("nickname")
        region = request.form.get("region")
        img_file = request.files.get("img")

        if nickname:
            current_user.nickname = nickname.strip()

        if region:
            current_user.region = region.strip()

        if img_file and img_file.filename:
            filename = secure_filename(img_file.filename)
            img_path = os.path.join(UPLOAD_FOLDER, filename)
            img_file.save(img_path)
            current_user.img = filename

        db.session.commit()
        flash("Profile updated successfully!", "success")
        return redirect(url_for("profile.profile"))

    return render_template("profile/profile.html", user=current_user)
