#!/usr/bin/python3

import cgi

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
print("<h1>Python CGI</h1>")
if name:
    print("<p>Hello! {}</p>".format(name))
else:
    print("<p>No form data submitted.</p>")
print("</body>")
print("</html>")
