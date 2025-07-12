from flask_wtf import FlaskForm
from flask_wtf.file import FileField, FileRequired, FileAllowed
from wtforms.fields import StringField, PasswordField, SubmitField
from wtforms.validators import DataRequired, Email, Length, EqualTo, Regexp

class RegisterForm(FlaskForm):
    first_name = StringField("First Name", validators=[DataRequired(), Length(min=4, max=100)])
    last_name = StringField("Last Name", validators=[DataRequired(), Length(min=2, max=100)])
    email = StringField("Email", validators=[DataRequired(), ])
    username = StringField("Username", validators=[DataRequired(), Length(min=4, max=15), Regexp(r'^[A-Za-z0-9_]*$', message="Username must only contain letters, numbers, and underscores.")])
    password = PasswordField("Password", validators=[DataRequired(), Length(min=8, max=20)])
    confirm_password = PasswordField("Confirm Password", validators=[DataRequired(), EqualTo('password', message="Passwords must match.")])
    submit = SubmitField("Register")

class LoginForm(FlaskForm):
    username = StringField("Username", validators=[DataRequired()])
    password = PasswordField("Password", validators=[DataRequired()])
    login = SubmitField("Login")

class Profile(FlaskForm):
    img = FileField("აირჩიეთ პროფილის ფოტო",        validators=[FileRequired(), FileAllowed(['jpg', 'png'], 'Only jpg and png images are allowed')]
)
    nickname = StringField()

    submit = SubmitField("შექმნა")
