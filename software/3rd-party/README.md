_______________________________________________________________________________
## 3rd-party

### Applications

- `putty` -- PuTTY -- client program for SSH, Telnet, and Rlogin network protocols
- `pscp` -- PuTTY SCP -- secure copy

### Instructions

Copy file to pi user's home directory:
```
  pscp myfile.txt pi@192.168.1.8:
```

Copy file to /home/pi/project/
```
   pscp myfile.txt pi@192.168.1.8:project/
```

Copy file from remote machine
```
  pscp pi@192.168.1.8:myfile.txt
```

### Copy multiple files

Separate files with spaces:
```
  pscp myfile.txt myfile2.txt pi@192.168.1.8:
```

Wildcard:
```
  pscp *.txt pi@192.168.1.8:
  pscp m* pi@192.168.1.8:
  pscp m*.txt pi@192.168.1.8:
```

### Alternative

FTP application such as FileZilla

_______________________________________________________________________________
