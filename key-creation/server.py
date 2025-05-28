from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse as urlparse
import sys
import threading
import uuid

project_id = ""
if len(sys.argv) > 1:
    project_id = sys.argv[1]
else:
    print('Project ID not provided.')
    sys.exit(1)
    
project_id = project_id+"_"+str(uuid.uuid4())

class OAuthCallbackHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        query_components = urlparse.parse_qs(urlparse.urlparse(self.path).query)
        if 'code' in query_components:
            authorization_code = query_components['code'][0]
            # Exchange the authorization code for an access token
            self.exchange_code_for_token(authorization_code)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'Authorization complete. You can close this window.')
        
        # Shutdown the server after handling the request
        threading.Thread(target=self.server.shutdown).start()

    def exchange_code_for_token(self, code):
        import requests
        token_url = 'https://git.prog2.de/oauth/token'
        client_id = 'ca35b0898503abc986f1eb5f536cded1c330d0d93818ada1422c41ee44af8fc5'
        client_secret = 'e6b0c6aa7ecc74f5a225f2b19d848279ad2700d0c489c0f2742bbc5e0afc2f1b'
        redirect_uri = 'http://localhost:8000/callback'
        data = {
            'client_id': client_id,
            'client_secret': client_secret,
            'code': code,
            'grant_type': 'authorization_code',
            'redirect_uri': redirect_uri
        }
        response = requests.post(token_url, data=data)
        if response.status_code == 200:
            access_token = response.json().get('access_token')
            # Use the access token to add the SSH key
            self.add_ssh_key(access_token)
        else:
            print('Failed to obtain access token:', response.text)

    def add_ssh_key(self, access_token):
        import requests
        ssh_key_path = '/root/.ssh/id_ed25519.pub'
        with open(ssh_key_path, 'r') as key_file:
            public_key = key_file.read().strip()
        api_url = 'https://git.prog2.de/api/v4/user/keys'
        headers = {'Authorization': f'Bearer {access_token}'}
        data = {'title': 'project'+project_id+'-key', 'key': public_key}
        response = requests.post(api_url, headers=headers, data=data)
        if response.status_code == 201:
            print('SSH key added successfully.')
        else:
            print('Failed to add SSH key:', response.text)

def run_server():
    server_address = ('', 8000)
    httpd = HTTPServer(server_address, OAuthCallbackHandler)
    print('Starting server at http://localhost:8000')
    httpd.serve_forever()

if __name__ == '__main__':
    run_server()
