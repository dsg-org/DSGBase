from flask import Blueprint, flash, redirect, render_template, url_for
from flask_login import current_user, login_required, login_user, logout_user
from werkzeug.security import check_password_hash, generate_password_hash

from pysrc.ext import db
from pysrc.models import User

from .forms import LoginForm, RegisterForm

auth = Blueprint("auth", __name__, template_folder="templates")


@auth.route("/login", methods=["GET", "POST"])
def login():
    if current_user.is_authenticated:
        return redirect(url_for("index"))

    login_form = LoginForm()
    register_form = RegisterForm()

    if login_form.validate_on_submit():
        user = User.query.filter_by(username=login_form.username.data).first()
        if user and check_password_hash(
            user.password, login_form.password.data
        ):
            login_user(user)
            return redirect(url_for("index"))
        flash("Invalid credentials", "danger")

    return render_template(
        "base.html",
        login_form=login_form,
        register_form=register_form,
        show_login_overlay=True,
    )


@auth.route("/register", methods=["GET", "POST"])
def register():
    register_form = RegisterForm()
    login_form = LoginForm()

    if register_form.validate_on_submit():
        first_name = register_form.first_name.data
        last_name = register_form.last_name.data
        email = register_form.email.data
        username = register_form.username.data
        password = register_form.password.data
        confirm_password = register_form.confirm_password.data

        if password != confirm_password:
            flash("Passwords do not match.", "error")
            return render_template(
                "base.html",
                login_form=login_form,
                register_form=register_form,
                show_register_overlay=True,
            )

        existing_user = User.query.filter(
            (User.username == username) | (User.email == email)
        ).first()

        if existing_user:
            flash("Username or email already exists!", "error")
            return render_template(
                "base.html",
                login_form=login_form,
                register_form=register_form,
                show_register_overlay=True,
            )

        hashed_password = generate_password_hash(password)

        user = User(
            first_name=first_name,
            last_name=last_name,
            email=email,
            username=username,
            password=hashed_password,
        )

        db.session.add(user)
        db.session.commit()
        flash("Registration successful! Please log in.", "success")
        return redirect(url_for("auth.login"))

    return render_template(
        "base.html",
        login_form=login_form,
        register_form=register_form,
        show_register_overlay=True,
    )


@auth.route("/logout")
@login_required
def logout():
    logout_user()
    return redirect(url_for("index"))
