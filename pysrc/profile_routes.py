import os

from flask import Blueprint, flash, redirect, render_template, request, url_for
from flask_login import current_user, login_required
from werkzeug.utils import secure_filename

from pysrc.ext import db
from pysrc.models import User

profile_bp = Blueprint("profile", __name__, template_folder="templates")

UPLOAD_FOLDER = "static/images"


@profile_bp.route("/profile", methods=["GET", "POST"])
@login_required
def profile():
    if request.method == "POST":
        nickname = request.form.get("nickname")
        district = request.form.get("district")
        img_file = request.files.get("img")

        if nickname:
            current_user.nickname = nickname.strip()

        if district:
            current_user.district = district.strip()

        if img_file and img_file.filename:
            filename = secure_filename(img_file.filename)
            img_path = os.path.join(UPLOAD_FOLDER, filename)
            img_file.save(img_path)
            current_user.img = filename

        db.session.commit()
        flash("Profile updated successfully!", "success")
        return redirect(url_for("profile.profile"))

    return render_template("profile/profile.html", user=current_user)


@profile_bp.route("/PageUser/<int:user_id>", methods=["POST"])
@login_required
def update_user_by_admin(user_id):
    if not current_user.is_admin() and current_user.id != user_id:
        flash("You are not authorized to edit this profile.", "danger")
        return redirect(url_for("index"))

    user = User.query.get_or_404(user_id)

    user.first_name = request.form.get("first_name", "").strip()
    user.last_name = request.form.get("last_name", "").strip()
    user.email = request.form.get("email", "").strip()
    user.district = request.form.get("district", "").strip()

    if current_user.is_admin():
        user.role = request.form.get("role", user.role)

    img_file = request.files.get("img")
    if img_file and img_file.filename:
        filename = secure_filename(img_file.filename)
        img_path = os.path.join("static/images", filename)
        img_file.save(img_path)
        user.img = filename

    try:
        db.session.commit()
        flash("Profile updated successfully!", "success")
    except Exception as e:
        db.session.rollback()
        flash("Error updating profile: " + str(e), "danger")

    return redirect(url_for("PageUser", user_id=user.id))
