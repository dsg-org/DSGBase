import os
from flask import Flask
from pysrc.ext import db, login_manager
from pysrc.models import User
from pysrc.routes import register_routes
from pysrc.auth_routes import auth
from pysrc.profile_routes import profile_bp

app = Flask(__name__)
app.config["SECRET_KEY"] = "kascbaSBAJBJ!@#3546"
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///database.db"


app.register_blueprint(profile_bp)

db.init_app(app)
login_manager.init_app(app)
login_manager.login_view = "auth.login"


@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))


app.register_blueprint(auth)
register_routes(app)


if not os.path.exists("./instance/database.db"):
    with app.app_context():
        db.create_all()
        print(" ბაზა და ცხრილები შეიქმნა.")

if __name__ == "__main__":
    app.run(debug=True)
