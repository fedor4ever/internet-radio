Each script attached gives help with option "-h" in the command line.

There are 3 different scripts:

 cenrep_keys_syntax_checker.pl 
      Used to verify the syntax of the excel sheets.

 gather_cenrep_to_storable.pl
      This reads the cenrep excel sheet(s) information into a storable file. This is
      a binary format, which can be used by other scripts.
      (for instance generate_cenrep_inifile.pl)
      Main motivation to use: reading the information from excel sheets takes 95%
      of time. If you for instance need to generate more than 1 confiration from the
      same data it makes sence to 1st run this script once and then the other
      script as many times as needed.

  generate_cenrep_inifile.pl
      This script is used to generate both cenrep ini-files based on the information
      in the excel sheets and also if needed a file for the RFS.

NOTES:
  - the generate_cenrep_inifile.pl requires that you have active perl 5.8.4 or newer version
    installed. Perl version 5.6.* does not work and results in error.
    Reason for the new version is the unicode support needed.

  - The scripts also require that Storable.pm module installed to the machine. With some
    earlier perl-versions this module was not part of default delivery.

  - There will be an error, if the path, where the excel-files are located, contains
    directory names with spaces in them.
    Solution: make sure that directory names in path don't contain spaces.

  - If a new key is added, the default value must be defined also in 
    CVRRepositoryManager::CheckRepositoryKeysL() to make sure that SIS installation works ok.

  - Note that cenrep folder must be writable!

Generating cenrep txt file:
  -In cenrep folder type:
    "attrib -r *.txt"  
    "generate_cenrep_inifile.pl -r 5.0 -d ."
  -In emulator:
     - delete .cre file with visualradio uid from epoc32\WINSCW\C\private\10202be9\persists\
     - type "abld export" to export cenrep file
