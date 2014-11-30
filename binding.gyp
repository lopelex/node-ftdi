{
  	"targets": [
    	{
		"target_name": "ftdi",
      		"sources": [ "src/node_ftdi.cpp" ],
      		"include_dirs": [ "/usr/include/" ],
      		"link_settings": { "libraries": [ "-l ftdi" ] }
		}
	]
}
