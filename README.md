# hidemyass

```
'##::::'##:'####:'########::'########:'##::::'##:'##:::'##::::'###:::::'######:::'######::
 ##:::: ##:. ##:: ##.... ##: ##.....:: ###::'###:. ##:'##::::'## ##:::'##... ##:'##... ##:
 ##:::: ##:: ##:: ##:::: ##: ##::::::: ####'####::. ####::::'##:. ##:: ##:::..:: ##:::..::
 #########:: ##:: ##:::: ##: ######::: ## ### ##:::. ##::::'##:::. ##:. ######::. ######::
 ##.... ##:: ##:: ##:::: ##: ##...:::: ##. #: ##:::: ##:::: #########::..... ##::..... ##:
 ##:::: ##:: ##:: ##:::: ##: ##::::::: ##:.:: ##:::: ##:::: ##.... ##:'##::: ##:'##::: ##:
 ##:::: ##:'####: ########:: ########: ##:::: ##:::: ##:::: ##:::: ##:. ######::. ######::
..:::::..::....::........:::........::..:::::..:::::..:::::..:::::..:::......::::......:::
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
轻轻地我走了     正如给我轻轻地来                  
        我轻轻地挥手     作别西边的云彩            
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```

This is a tool for wiping access log when you really wanna hide yourself from admin.
We're modifying those systemlog very carefully by removing one single log record 
instead of the whole log file. Also, the file permission, owner/group and ctime/atime 
are kept as the old file.

# Usage

```
Usage: ./hidemyass [ENTRIES] [FILTERS] ACTIONS        
ENTRIES:                                
  -u, --utmp=utmp_file                
      specify the path to utmp file, which is /var/run/utmp by default 
      utmp file is read by 'who','w' and other commands 
  -w, --wtmp=wtmp_file                
      specify the path to wtmp file, which is /var/log/wtmp by default 
      wtmp is read by 'last' and other commands
  -b, --btmp=btmp_file                
      specify the path to btmp file, which is /var/log/btmp by default 
      btmp is read by 'lastb' and other commands
      for some systems the bad login attempts are written to
      /var/log/auth.log or /var/log/secure instead of btmp
  -l, --lastlog=lastlog_file          
      specify the path to lastlog file, which is /var/log/lastlog by default 
      lastlog is read by 'lastlog' and other commands
      note the only valid FILTERS for lastlog is username(-n)
FILTERS:                                
  -n, --name=username                   
      specify log record by username 
  -a, --address=host                    
      specify log record by host ip address  
  -t, --time=time                       
      specify log record by time (YYYY:MM:DD:HH:MM:SS) 
ACTIONS: 
  -p, --print                           
      print records for specified ENTRIES 
  -c, --clear                           
      clear records for specified ENTRIES with FILTERS
      usually you need permission to tamper logs 
  -h, --help                            
      show this message and exit
 
```

# Examples

1. print utmp records

./hidemyass -u -p

2. print utmp records in other path

./hidemyass --utmp=/var/adm/utmpx -p

3. print all records

./hidemyass -uwbl -p

4. modify lastlog record for user root to time 2017/04/01 13:26:00

./hidemyass -l -n root -t 2017:04:01:13:26:00 -c

5. clean all tmpx records that from ip 220.181.57.217

./hidemyass -uwb -a 220.181.57.217 -c


# TODO

## some other logs to clean

- /var/log/auth.log
- /var/log/secure
- /var/log/maillog


Since you could modify system log, that usually means you have already got
privilege escalation. As a result, you may want to clear other logs too, 
such as `/var/log/syslog`, `/var/log/dmesg`, `/var/log/messages` and some 
application crash logs.
