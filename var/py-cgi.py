#!/usr/bin/python3

import cgi
import datetime

# Get form data
form = cgi.FieldStorage()
name = form.getvalue('name')

# Generate HTML page
print("Content-type: text/html\r\n\r\n", end="")
print("<html>")
print("<head>")
print("<title>Python CGI</title>")
print("</head>")
print("<body>")
print("<div style=\"text-align: center;\">")
print("<p>Python CGI</p>")
if name:
    print("<h1>Hello! {}</h1>".format(name))
else:
    print("<p>No form data submitted.</p>")
d = datetime.datetime.now()
print("<p>Today: ")
print(d.year,'-',d.month,'-',d.day)
print("</p>")
print("</div>")
print("</body>")
print("</html>")
