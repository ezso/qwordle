import webbrowser

client_id = 'ca35b0898503abc986f1eb5f536cded1c330d0d93818ada1422c41ee44af8fc5'
redirect_uri = 'http://localhost:8000/callback'
auth_url = f'https://git.prog2.de/oauth/authorize?client_id={client_id}&redirect_uri={redirect_uri}&response_type=code&scope=api'
webbrowser.open(auth_url)
