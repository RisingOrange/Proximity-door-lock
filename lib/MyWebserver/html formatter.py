htmlFile = open('config_site.html')
html = htmlFile.read()
htmlFile.close()

if not ("|LIST|" in html and "|IP|" in html):
    print("terminated process with an error")
    print("the placeholders are missing")
    exit()

#because the c++ compiler interprets the string
html = html.replace('\"', '\\\"')
html = html.replace('\\\'', '\\\\\\\'') #this e.g. for "can't"

result = "#define webpage "
result += "\""
result += "\\"
result += "\n"
for line in html.splitlines():
    result += line
    result += "\\"
    result += "\n"
    result += "\\n"
result += "\\"
result += "\n"
result += "\""

outputFile = open('webpage.h', 'w')
outputFile.write(result)
outputFile.close()

print('saved result in file')
