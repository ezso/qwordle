# Use your host key (one key to rule all projects)
To reuse the host ssh-key:
(terminal.integrated.inheritEnv has to be true)

Linux/Mac: 
```
eval $(ssh-agent -s)
ssh-add
code .
```
Windows: https://stackoverflow.com/questions/18683092/how-to-run-ssh-add-on-windows/18683544#18683544


# Alternative (one key per container/project)
We create a new key and add it to our user:
```
cd key-creation
./add-key.sh
```