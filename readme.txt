



Install service using the following command in an elevated Command Prompt:

sc create HassMeService ^
 binPath= "C:\Program Files\HassMeService\hassme.exe" ^
 start= auto ^
 DisplayName= "HassMe Service"

To check the status of the service, use:
sc query HassMeService

To start the service, use:
sc start HassMeService

To stop the service, use:
sc stop HassMeService

To delete the service, use:
sc delete HassMeService
