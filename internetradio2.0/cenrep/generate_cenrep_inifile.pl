#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
######################################
# Run with -h to get help
######################################
use strict;
use lib   ".";
use open OUT => ':raw';
use Win32::OLE qw(in with);
use Win32::OLE::Const 'Microsoft Excel';
use Cwd;
use Getopt::Long;
use File::Copy;
use Storable qw(fd_retrieve);
use excel_support;  # Own perl module that contains the reading of memory-sheets
use cenrep_keys;  # Own perl module that contains the reading of memory-sheets


use Data::Dumper;

# this is the bit mask that is put into the metadata in the ini-file, when
# backup is set on.
my $globalBackupValue = 0x01000000;

###########################################################################
###########################################################################
sub printStr
{
    my $to = shift;
    my $str = shift;

    for (my $i = 0; $i < length($str); $i++) 
    {
	printf $to "%s\0", substr($str,$i, 1);
    }
}

###########################################################################
###########################################################################
sub printNewLine
{
    my $to = shift;
    my $fileName = shift;
    
    print $to "\x0D\x00\x0A\x00";

    return;
}
#      # We need to close the file temporarely so that we can
#      # write a new line into it differently. It seems that
#      # unicode file name writing is somehow wrong in the 
#      # used perl versio (ActivePerl 5.8.4)
    
#      close $to;
#      # Writing the new line into the end of the line
#      open $to,">>$fileName";
#      binmode OUT;
#      print OUT chr(13);
#      print OUT chr(0);
#      print OUT chr(10);
#      print OUT chr(0);
#      close OUT;
    
#      # Opening the file again to
#  #    open(OUT,">>:encoding(UTF-16LE)",$fileName) || die "unable to open $fileName";
#      open(OUT,">>$fileName") || die "unable to open $fileName";
#  }

###########################################################################
# Function that gets a dir as parameter and reads the list of excel files from the
# given directory.
#
# Params: a directory name
#
# Return value: a reference to a list of filenames.
###########################################################################
sub getExcelFilesFromDir
{
    my $dir = shift;
    
    # the given dir should be relative to the dir, where the script is executed.
    if($dir !~ /^[\\\/]/ && $dir !~ /^[A-Za-z]:/)
    {
	$dir =  cwd . "\\$dir"; 
    }
    my @files;
    @files = <$dir\\*.xls >;
   
    return \@files;
}


###################################################
# Function that reads the file names from the given file.
# THe information is read from Column A and 1st empty row indicates
# end of input.
#
# Params: fileNameRead, refToExcelApp
###################################################
sub getExcelFilesFromFile
{
    my $file = shift;
    
    my @fileList;
    # ----------------------------------------------------------------  
    # Open the file
    # ----------------------------------------------------------------  
    open(IN,$file) || die "Unable to open: $file";

    #------------------------------------------------------------
    # Read file line by line
    #------------------------------------------------------------
    while(<IN>)
    {
	my $fileWithPath = $_;
	chomp($fileWithPath);

	if(! -f $fileWithPath)
	{
	    print STDERR "Warning: input file not found: $fileWithPath\n";
	}
	else
	{
	    push(@fileList,$fileWithPath);
	}
    }
	
    return \@fileList;
}

###################################################
###################################################
sub isRemoved
{
    my $platId = shift;
    my $platRel = shift;
    my $removed = shift;

    # If not defined => not removed
    if(! defined($removed))
    {
	return 0;
    }
   
    if($removed =~ /$platId\s+([0-9]+\.[0-9]+)/i)
    {
	my $rel = $1;
	if($rel <= $platRel)
	{
	    return 1;
	}
    }
    return 0;    
}
###################################################
# returns the official capa name from internal name.
# this is implemented since the capa names in cenrep keys
# migth not be the same as official names in the ini-file.
###################################################
sub getCapaValue
{
    my $formatNbr      = shift; # not used at the moment.
    my $refToCapaList  = shift;

    #########################
    # official way, caps as strings
    #########################

#      my $retVal = "";
#      my $isFirst = 1;
#      foreach my $capa (@$refToCapaList)
#      {
#  	next if($capa =~ /^\s*$/);
#  	if(!$isFirst)
#  	{
#  	    $retVal .= ",";
#  	}
#  	$retVal .= $capa;

#  	$isFirst = 0;
#      }

    #########################
    # Caps as strings
    #########################
    my $retVal = "";
    my @capaList;
    foreach my $capa (@$refToCapaList)
    {
	next if($capa =~ /^\s*$/);

	if( $capa =~ /ReadUserData/)
	{
	    push(@capaList,"ReadUserData"); 
	}
	elsif($capa =~ /WriteUserData/)
	{
	    push(@capaList,"WriteUserData");
	}
	elsif( $capa =~ /ReadDeviceData/)
	{
	    push(@capaList,"ReadDeviceData");
	}
	elsif($capa =~ /WriteDeviceData/)
	{
	    push(@capaList,"WriteDeviceData");
	}
	elsif($capa =~ /NetworkControl/)
	{
	    push(@capaList,"NetworkControl");
	}
	elsif($capa =~ /NetworkServices/)
	{
	    push(@capaList,"NetworkServices");
	} 
	elsif($capa =~ /Local Services/)
	{
	    push(@capaList,"LocalServices");
	} 
	elsif($capa =~ /Location/)
	{
	    push(@capaList,"Location");
	} 
	elsif($capa =~ /ProtServ/)
	{
	    push(@capaList,"ProtServ");
	} 
	elsif($capa =~ /DRM/)
	{
	    push(@capaList,"DRM");
	} 
	elsif($capa =~ /SwEvent/)
	{
	    push(@capaList,"SwEvent");
	} 
	elsif($capa =~ /PowerMgmt/)
	{
	    push(@capaList,"PowerMgmt");
	} 
	elsif($capa =~ /AllFiles/)
	{
	    push(@capaList,"AllFiles");
	} 
	elsif($capa =~ /DiskAdmin/)
	{
	    push(@capaList,"DiskAdmin");
	} 
	elsif($capa =~ /MultiMediaDD/)
	{
	    push(@capaList,"MultimediaDD");
	} 
	elsif($capa =~ /CommDD/)
	{
	    push(@capaList,"CommDD");
	} 
	elsif($capa =~ /TrustedUI/)
	{
	    push(@capaList,"TrustedUI");
	} 
	elsif($capa =~ /TCB/)
	{
	    push(@capaList,"TCB");
	} 
	elsif($capa =~ /AlwaysPass/  || $capa =~ /None/)
	{
	    push(@capaList,"alwayspass");
	} 
	elsif($capa =~ /UserEnvironment/)
	{
	    push(@capaList,"UserEnvironment");
	} 
	elsif($capa =~ /SurroundingsDD/)
	{
	    push(@capaList,"SurroundingsDD");
	} 
    }
    
    return join(",",@capaList);

}
###################################################
# 3rd param syntax:
# <release> 
#     => { 
#        "R" => (<capa1ForReading>,<capa2ForReading>)
#        "W" => (<capa1ForWriting>,<capa2ForWriting>)
#        "value" => <value>
#        "rfs"  => <0/1>
#        "backup"  => <0/1>
#         }
#  
###################################################
sub getConfig
{
    my $platId = shift;
    my $platRel = shift;
    my $refToHash = shift;

    my $currRel = "0";
    my $currRef = undef;
    foreach my $plat (sort keys %$refToHash)
    {
	if($plat =~ /$platId\s+([0-9]+\.[0-9]+)/i)
	{
	    my $tentativeRel = $1;
	    if($tentativeRel > $currRel && $tentativeRel <= $platRel)
	    {
		$currRel = $tentativeRel;
		$currRef = $$refToHash{$plat};
	    }
	}
    }
#      if($currRel > 0)
#      {
#  	print $$refToHash{$currRel}, "a\n";
#  	return $$refToHash{$currRel};
#      }
    return $currRef;
}

###########################################################################
# Main function, 
###########################################################################
sub readConfig
{
    my $refToExcelFiles = shift;
    my $refToExcel = shift;
    
    my %reshHash;
    my $refToWorkSheet;
    
    # Loop through each found excel sheet. This will open one file after each other
    # and parse information into the hashes defined above.
    foreach my $file ( @{$refToExcelFiles} ) 
    {
	print "   Reading: $file\n";

	# ----------------------------------------------------------------  
	# Open the file
	# ----------------------------------------------------------------  
	my $refToWb = openWorkbook( $file,$refToExcel);

	# ----------------------------------------------------------------  
	# Check, that the template version is the one we support. If not
	# the file is skipped and warning is written to user.
	# ----------------------------------------------------------------  
#  	if( isSupported($refToWb) == 0)
#  	{
#  	    # "delete" the workbooks, this closes the excel application
#  	    undef $$refToWb;
#  	    undef $refToWb;
#  	    print STDERR "Warning: Unsupported template version in file: $file\n";
#  	    next;
#  	}

	
	# ----------------------------------------------------------------  
	# Read the information from the worksheets
	# ----------------------------------------------------------------
	for( my $i = 0; $i < $$refToWb->Worksheets->Count; $i++)
	{
	    my $workSheet = $$refToWb->WorkSheets($i + 1);
	    $refToWorkSheet = getWorkSheet( $refToWb,$workSheet->Name);
	    readCentrepKeyInfo( $refToWorkSheet,\%reshHash);
	}
	# "delete" the workbooks, this closes the excel application
	$$refToWb->Close(0);
	undef $$refToWb;
	undef $refToWb;
    }
    return \%reshHash;
}
###########################################################################
###########################################################################
sub getKeyType
{
    my $key = shift;

    return "string" if($key =~ /str/i);
    return "int" if($key =~ /int/i);
    return "real" if($key =~ /real/i);
    return "binary" if($key =~ /bin/i);

    return "unknown_type";
}


###########################################################################
# Prints the Key and key type into the given output.
# The format in which the key is printed is format specific.
###########################################################################
sub printKey
{
    my $to = shift;
    my $formatNbr = shift;
    my $key = shift;
    my $keyType = shift;

    $key =~ s/0x(0*)/0x/g;
    # Add 0 if the key value is "0x" (cases, where the id == 0)
    $key .= "0" if $key =~ /0x\s*$/;
    printStr $to, $key . " " . $keyType . " ";

    return;
}

###########################################################################
# Prints the capabilities into the given output.
# The format in which the capabilities are printed is format specific.
###########################################################################
sub printCapa
{
    my $to = shift;
    my $formatNbr = shift;
    my $refToCapaList = shift;
    my $prefix = shift;
    my $alwaysFail = shift;

    if($formatNbr > 1)
    {
	if($alwaysFail && $formatNbr > 3)
	{
	    printStr \*OUT, " $prefix=alwaysfail";
	}
	elsif(defined($refToCapaList) && scalar(@$refToCapaList) > 0)
	{
	    my $capaStr = getCapaValue($formatNbr,$refToCapaList);
	    printStr \*OUT, " $prefix=$capaStr" if($capaStr);
	}
    }
}

###########################################################################
# Prints the Sid  capabilities into the given output.
# The format in which the capabilities are printed is format specific.
###########################################################################
sub printSidCapa
{
    my $to = shift;
    my $formatNbr = shift;
    my $refToCapaList = shift;
    my $prefix = shift;

    if($formatNbr > 1)
    {
	if(defined($refToCapaList) && scalar(@$refToCapaList) > 0)
	{
	    # Only 1 SID is allowed
	    printStr \*OUT, " $prefix=$$refToCapaList[0]";
	}
    }
}

###########################################################################
# Prints the information about ranges
###########################################################################
sub printPlatSec
{
    my $to = shift;
    my $formatNbr = shift;
    my $refToUidSpecificHash = shift;
    my $refToRangesHash = shift;
    my $fileName = shift;
    my $addDefCaps = shift;

    #-------------------------------
    # UID specific (default capabilities)
    #-------------------------------
    # we need to know if write / read was specified.
    # if not and addDefCaps is defined => then be add the 
    # default alwayspass to the file
    my $readSpecified = 0;
    my $writeSpecified = 0;

    ###############################
    # RSid-capa writing (format specific)
    ###############################
    my $refToCapaList = $$refToUidSpecificHash{"RSid"};
    printSidCapa($to,$formatNbr,$refToCapaList,"sid_rd");
    $readSpecified += scalar(@$refToCapaList) if defined($refToCapaList);
    
    ###############################
    # R-capa writing (format specific)
    ###############################
    $refToCapaList = $$refToUidSpecificHash{"R"};
    printCapa($to,$formatNbr,$refToCapaList,"cap_rd",0);
    $readSpecified = scalar(@$refToCapaList) if defined($refToCapaList);
    
    # Add def caps if no caps were defined
    if($addDefCaps > 0 && $readSpecified < 1)
    {
	printStr \*OUT, " cap_rd=alwayspass";

    }

    ###############################
    # WSid-capa writing (format specific)
    ###############################
    $refToCapaList = $$refToUidSpecificHash{"WSid"};
    printSidCapa($to,$formatNbr,$refToCapaList,"sid_wr");
    $writeSpecified += scalar(@$refToCapaList) if defined($refToCapaList);
    
    ###############################
    # W-capa writing (format specific)
    ###############################
    $refToCapaList = $$refToUidSpecificHash{"W"};
    printCapa($to,$formatNbr,$refToCapaList,"cap_wr",0);
    $writeSpecified = scalar(@$refToCapaList) if defined($refToCapaList);
    
    # Add def caps if no caps were defined
    if($addDefCaps > 0 && $writeSpecified < 1)
    {
	printStr \*OUT, " cap_wr=alwayspass";

    }

    printNewLine($to,$fileName);
    
    #-------------------------------
    # Ranges
    #-------------------------------
    foreach my $rangeFrom (keys %$refToRangesHash)
    {
	my $refToRangeFromHash = $$refToRangesHash{$rangeFrom};

	foreach my $rangeTo (keys %$refToRangeFromHash)
	{
	    my $refToRangeToHash = $$refToRangeFromHash{$rangeTo};

	    printStr $to, $rangeFrom . " " .$rangeTo;
	    
	    ###############################
	    # RSid-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToRangeToHash{"RSid"};
	    printSidCapa($to,$formatNbr,$refToCapaList,"sid_rd");
	    
	    ###############################
	    # R-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToRangeToHash{"R"};
	    printCapa($to,$formatNbr,$refToCapaList,"cap_rd",0);
	
	    ###############################
	    # WSid-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToRangeToHash{"WSid"};
	    printSidCapa($to,$formatNbr,$refToCapaList,"sid_wr");
	    
	    ###############################
	    # W-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToRangeToHash{"W"};
	    printCapa($to,$formatNbr,$refToCapaList,"cap_wr",0);
	    
	    printNewLine($to,$fileName);
	}
    }    
}

###########################################################################
# Prints the information about metadata
###########################################################################
sub printMetadata
{
    my $to = shift;
    my $formatNbr = shift;
    my $refToUidSpecificHash = shift;
    my $refToRangesHash = shift;
    my $fileName = shift;

    #-------------------------------
    # UID specific metadata
    #-------------------------------

    my $metaData = 0;
    if($$refToUidSpecificHash{"backup"})
    {
	$metaData += $globalBackupValue;
    }
    printStr $to, " $metaData";
    printNewLine($to,$fileName);

    
    #-------------------------------
    # Ranges
    #-------------------------------
    foreach my $rangeFrom (keys %$refToRangesHash)
    {
	my $refToRangeFromHash = $$refToRangesHash{$rangeFrom};

	foreach my $rangeTo (keys %$refToRangeFromHash)
	{
	    my $refToRangeToHash = $$refToRangeFromHash{$rangeTo};

	    printStr $to, $rangeFrom . " " .$rangeTo;

	    my $metaData2 = 0;
	    if($$refToRangeToHash{"backup"})
	    {
		$metaData2 += $globalBackupValue;
	    }
	    printStr $to, " $metaData2";
	    printNewLine($to,$fileName);
	}    
    }
}

###########################################################################
# Function that generates a text file for the Restore factory settings.
# This file 
###########################################################################
sub generateRfsFile
{
    my $reshHash = shift;
    my $rfsFileWithPath = shift;

    open(OUT,">$rfsFileWithPath") || die "unable to open $rfsFileWithPath for writing";
    print OUT "\xFF\xFE";

    foreach my $uid (keys %$reshHash)
    {
	my $refToUidHash = $$reshHash{$uid};
	$uid =~ s/^0x0*//g;
	$uid = 0 if $uid =~ /^$/;
	my $refToUidSpecificHash = $$refToUidHash{"uid_specific"};

	# Uid specific rfs overwrites the key specific values
	# Thus if this is set to yes => we can skip key specific values
	if($$refToUidSpecificHash{"rfs"})
	{	    
	    printStr \*OUT, "CR $uid";
	    printNewLine(\*OUT,$rfsFileWithPath);
	    next;
	}

	# If we get here => uid was not set to yes for RFS.
	# now we loop through each ke
	########################
	# print each key into the file.
	########################
	my $refToKeysHash = $$refToUidHash{"keys"};
	my $isFirstInUid = 1;
	foreach my $key (sort keys %$refToKeysHash)
	{		
	    my $refToKeyHash = $$refToKeysHash{$key};

	    # Check, if the key was restored in RFS. 	    
	    if($$refToKeyHash{"rfs"})
	    {
		# If this is the 1st key for this UI, then
		# we print the keyword (CR) and the uid into the
		# file.
		if($isFirstInUid)
		{
		    $isFirstInUid = 0;
		    printStr \*OUT, "CR $uid";
		    
		}
		
		# print the key into the file (after some modifications)
		$key =~ s/^0x0*//g;
		$key = 0 if $key =~ /^$/;
		printStr \*OUT, " $key";
	    }
	}
	# Print a newline if (and only if) there was keys that had rfs-flag
	# on.
	if(! $isFirstInUid)
	{
	    printNewLine(\*OUT,$rfsFileWithPath);
	}
    }  
    close OUT;
}

###########################################################################
# Function that generates all the ini (txt)-files for the central repository.
# There are multiple formats, which are supported at the moment, since the
# development in ongoing and syntax is still a little but unknown.
###########################################################################
sub generateIniFiles
{
    my $reshHash = shift;
    my $dirname = shift;
    my $formatNbr = shift;
    my $addDefCaps = shift;

    my $iniFileCount = 0;
    my $keyCount = 0;

    foreach my $uid (keys %$reshHash)
    {
	my $refToHash = $$reshHash{$uid};
	$refToHash = $$refToHash{"keys"};
	my $refToRangeHash = $$refToHash{"ranges"};
	if(scalar(keys %$refToHash) < 1 && scalar(keys %$refToRangeHash))
	{
	    # Skip uid, if no keys and ranges
	    next;
	}
	$iniFileCount++;

	##################################
	# Filename syntax is "uid.txt" without the 0x in the begining 
	##################################
	my $fileName = "$uid.txt";
	$fileName =~ s/^0x//i;
			     
	# Add dir name into the filename. 
	if($dirname !~ /^\s*$/)
	{
	    my $tmpFileName = $fileName;
	    $fileName = $dirname;
	    if($dirname !~ /[\\\/]$/)
	    {
		$fileName .=  "\\";
	    }
	    $fileName .= $tmpFileName;
	}

	########################
        # Copy the start of the file into the new name
	# (note that this migth become format specific, 
	#  but is not at the moment).
	########################
	my $templateName = "cenrep_start.ini";
	    
	copy($templateName,$fileName) or die "Copy failed: $! : $fileName";
	open(OUT,">>$fileName") || die "unable to open $fileName";
	########################
	# Print the [owner] section
	########################
	if($formatNbr > 2)
	{
	    my $refToUidHash = $$reshHash{$uid};
	    my $refToUidSpecificHash = $$refToUidHash{"uid_specific"};

	    if($$refToUidSpecificHash{"owner"} !~ /^\s*$/)
	    {
		printStr \*OUT, "[owner]";
		printNewLine(\*OUT,$fileName);
		printStr \*OUT, $$refToUidSpecificHash{"owner"};
		printNewLine(\*OUT,$fileName);
	    }	    
	}

	########################
	# Print the [defaultmeta] section
        # Note that depending on the format
        # nbr the metadata has to be before or after
        # platsec-section.
	########################
	if($formatNbr > 3 )
	{
	    printStr \*OUT, "[defaultmeta]";
	    printNewLine(\*OUT,$fileName);

	    my $refToUidHash = $$reshHash{$uid};
	    printMetadata(\*OUT,$formatNbr,$$refToUidHash{"uid_specific"},
			  $$refToUidHash{"ranges"},$fileName);
	}
        
	########################
	# Print the [PlatSec] section
	########################
	if($formatNbr > 1)
	{
	    printStr \*OUT, "[platsec]";
	    printNewLine(\*OUT,$fileName);
    
	    my $refToUidHash = $$reshHash{$uid};
	    printPlatSec(\*OUT,$formatNbr,$$refToUidHash{"uid_specific"},
			$$refToUidHash{"ranges"},$fileName,$addDefCaps);
	}
	########################
	# Print the [metadata] section
	########################
	if($formatNbr eq 3)
	{
	    printStr \*OUT, "[metadata]";
	    printNewLine(\*OUT,$fileName);

	    my $refToUidHash = $$reshHash{$uid};
	    printMetadata(\*OUT,$formatNbr,$$refToUidHash{"uid_specific"},
			  $$refToUidHash{"ranges"},$fileName);
	}

	########################
	# Print the [Main] section
	########################
	if($formatNbr > 0)
	{
	    printStr \*OUT, "[Main]";
	    printNewLine(\*OUT,$fileName);
	}

	########################
	# print each key into the file.
	########################
	foreach my $key (sort keys %$refToHash)
	{		
	    $keyCount++;
	    my $refToHash2 = $$refToHash{$key};

	    ###############################
	    # Writing the key and key type. 
	    # This is format specific.
	    ###############################
	    printKey(\*OUT,$formatNbr,$key,getKeyType($$refToHash2{"key type"}));

	    ###############################
	    # Write the value into the file
	    ###############################

	    # Strings are always enclosed with \". Also empty binary value is also enclosed
	    # such way
 	    if($$refToHash2{"key type"} =~ /str/i ) 
	    {
		printStr \*OUT, "\"";
	    }
	    printStr \*OUT, $$refToHash2{"value"}; 
 	    if($$refToHash2{"key type"} =~ /str/i) 
	    {
		printStr \*OUT, "\"";
	    }

            if ($$refToHash2{"key type"} =~ /bin/i && $$refToHash2{"value"} =~ /^\s*$/)
            {
                printStr \*OUT, "00";
            }

	    ###############################
	    # Generating the metadata and adding it into the file
	    ###############################
	    my $metaData = 0;
	    if($$refToHash2{"backup"})
	    {
		$metaData += $globalBackupValue;
	    }
	    printStr \*OUT, " $metaData";

	    # Note, according to the cenrep specification, then read access has to be
	    # before the write access.

	    ###############################
	    # RSid-capa writing (format specific)
	    ###############################
	    my $refToCapaList = $$refToHash2{"RSid"};
	    printSidCapa(\*OUT,$formatNbr,$refToCapaList,"sid_rd");

	    ###############################
	    # R-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToHash2{"R"};
	    printCapa(\*OUT,$formatNbr,$refToCapaList,"cap_rd",0);

	    ###############################
	    # WSid-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToHash2{"WSid"};
	    printSidCapa(\*OUT,$formatNbr,$refToCapaList,"sid_wr");

	    ###############################
	    # W-capa writing (format specific)
	    ###############################
	    $refToCapaList = $$refToHash2{"W"};
	    printCapa(\*OUT,$formatNbr,$refToCapaList,"cap_wr",$$refToHash2{"read-only"});

	    ##########################################
	    ##########################################
	    ##########################################
	    ##########################################
	    printNewLine(\*OUT,$fileName);
	}
	close OUT;
    }

    print "Ini file count = $iniFileCount\n";
    print "Key count      = $keyCount\n";
}

###########################################################################
# Shows help.
#
# Params: -
#
# Return: -
#
###########################################################################
sub usage
{
    $0 =~ /([0-9a-zA-Z\_\.]+)$/;
    my $filename = $1;
    print "\n";
    print "\n";
    print "\n";
    print "This script is intended to be used to generate ini-files for Central Repository\n";
    print "based on the information in the cenrep key excel-sheet(s). These sheets define\n";
    print "the content of the ini-files.\n";
    print "\n";
    print "Rough discruption on how the values for the ini-files are determinated:\n";
    print "1) Content of sheets identified by -d or -s are read.\n";
    print "   (alternatively -b can be used a storable file, where the sheet info\n";
    print "    is already in binary format)\n";
    print "2) From sheets read in step 1 the information is parsed based on the\n";
    print "   information specified by -p, -r and -v\n";
    print "3) [optional] Default values specified by the steps 1 and 2 can be\n";
    print "    overriden by defining information in other sheets, specified by parameters\n";
    print "    -vd,-vs,-vb and -vv, \n";
    print "\n";
    print "\n";
    print "Usage:\n";
    print "  $filename [-h|-help] [-fh]\n";
    print "                [-p <platId>] -r <platRel> [-v <variantName]\n";
    print "                (-d <dirName> | -s <sourceFilename> | -b <storableFilename>)\n";
    print "                [ -rd <resultDir>]\n";
    print "                [ (-vd <variantDirName> | -vs <sourceFilename> | -vb <storableFilename>)\n";
    print "                  [-vv <variantsVariantName>]]\n";
    print "                [-f <formatNbr> ]\n";
    print "                [-dc ]\n";
    print "                [-rfs <dirName> [-noini]]\n";
    print "\n";
    print "Options:\n";
    print " -h                  : Show this help\n";
    print " -help               : Show this help\n";
    print " -fh                 : Show the help about supported formats.\n";
    print " -p <platId>         : The identifier specifying platform. Optional. Default value: S60\n"; 
    print "                       to the directory where the mem-sheets exist.\n";
    print " -r <platRel>        : The platform number. For example: 3.0\n";
    print " -rd <resultDir>     : The path, where the generated ini-files are written.\n";
    print "                        Default: current directory\n";
    print " -v <variantName>    : The variant name. Optional. Default value: \"\" == no variant.\n";
    print "                       The names are not fixed in the script. The same names has to be\n";
    print "                       used as supported by the excel sheet.\n";
    print " -d <dirName>        : The dir, which contains the sheets, which are read. Relative path to the\n";
    print "                       current dir or absoluth path. NOTE: the dir name can't contain any spaces.\n";
    print " -s <sourceFilename>       : The text filename, which contains sheet name with absoluth\n";
    print "                             path names (1/row)\n";
    print " -b <storableFilename>  : the storable file name, which contains the same information as the\n";
    print "                          key sheets. ( this can be generated with gather_cenrep_to_storable.pl)\n";
    print " -vd <variantDirName>      : Same as -d, but for variant information\n";
    print "                             (see rough description above) Optional.\n";
    print " -vs <sourceFilename>      : Same as -s, but for variant information\n";
    print "                              (see rough description above) Optional.\n";
    print " -vb <storableFilename>  : the storable file name, which contains the same information as the\n";
    print "                          variant key sheets.\n";
    print " -vv <variantsVariantName> : Same as -v, but for variant information\n";
    print "                              (see rough description above) Optional.\n";
    print " -f <formatNbr>          : the format type that is generated. (see list of formats with -fh)\n";
    print " -dc                     : adds alwayspass as default capabilities for UID namespace if non \n";
    print "                              specified (also applies to certain formats)\n";
    print " -rfs <dirName>        : The dir to which cenrep.txt-file is generated. This file\n";
    print "                         contains info for Restore Factory Settings\n";
    print " -noini                : used only with -rfs-option. If specified, only RFS file generated,\n";
    print "                          no ini-files.\n";
    print "\n";
    print "\n";
    print "You have specify either -d or -s, but not both at the same time.\n";
    print "You can specify either -vd or -vs, but not both at the same time.\n";
    print "\n";
    print "\n";
}


###########################################################################
# Shows format help.
#
# Params: -
#
# Return: -
#
###########################################################################
sub formatHelp
{
    $0 =~ /([0-9a-zA-Z\_\.]+)$/;
    my $filename = $1;
    print "\n";
    print "\n";
    print "Supported formats (default format is 4):\n";
    print "   0 : Format, where there are no capabilities generated for the keys.\n";
    print "       Supported is key (as hex), type, value and metadata.\n";
    print "       No \"[Main]\"-tag is included into the ini-file.\n";
    print "       (a temporary fix so that testing can continue).\n";
    print "   1 : Format 0 + \"[Main]\"-tag is included into the ini-file.\n";
    print "       (Symbian w48 and earlier builds).\n";
    print "\n";
    print "   2 : Format 1 + in addition  capabilities are written in text format.\n";
    print "       Supported is key (as hex), type, value, metadata and read/write capabilities in text format.\n";
    print "       (Symbian w50 and later builds).\n";
    print "\n";
    print "   3 : Format 3 + in addition owner and metadata are written in text format.\n";
    print "       Supported is key (as hex), type, value, metadata and read/write capabilities in text format.\n";
    print "       (Symbian w?? and later builds).\n";
    print "\n";
    print "   4 : Format 4 + in addition alwaysfail-keyword used in ini-file.\n";
    print "       (Symbian w10 and later builds).\n";
    print "\n";
    print "\n";
    print "\n";

}
###########################################################################
# Parses the command line parameters from ARGV
#
# Params: -
#
# Return: -		    
#
###########################################################################
sub parseCmdLine
{
    my $platId = "S60";
    my $platRel = "";
    my $varName = "";
    my $sheetDirname = "";
    my $sheetFilename = "";
    my $storableFilename = "";
    my $resultDirname = "";
    my $varNameVariant = "";
    my $sheetDirnameVariant = "";
    my $sheetFilenameVariant = "";
    my $storableFilenameVariant = "";
    my $formatNbr = -1;
    my $addDefCaps = 0;
    my $rfsDir = "";
    my $noIni = 0;
    my $showHelp = 0;
    my $showFormatHelp = 0;

    if( ! GetOptions('p:s'      => \$platId,
		     'r:s'      => \$platRel,
		     'v:s'      => \$varName,
		     'd:s'      => \$sheetDirname,
		     's:s'      => \$sheetFilename,
		     'b:s'      => \$storableFilename,
		     'rd:s'     => \$resultDirname,
		     'vv:s'     => \$varNameVariant,
		     'vd:s'     => \$sheetDirnameVariant,
		     'vs:s'     => \$sheetFilenameVariant,
		     'vb:s'     => \$storableFilenameVariant,
		     'f:i'      => \$formatNbr,
		     'dc'       => \$addDefCaps,
		     'rfs:s'    => \$rfsDir,
		     'noini'    => \$noIni,
		     'h'        => \$showHelp,
		     'help'     => \$showHelp,
		     'fh'       => \$showFormatHelp,
		     '<>'       => \&usage))
    {
	usage();
	exit(1);
    }
    

    ####################################
    # Format number handling.
    ####################################
    # Set default format number, if not specified.
    if( $formatNbr < 0)
    {
	$formatNbr = 4;
    }
    elsif( $formatNbr > 4)
    {
	print "\nError: Invalid format number: $formatNbr.\n";
	$showFormatHelp = 1;
    }

    if($showFormatHelp)
    {
	formatHelp();
	exit(1);
    }
    if($showHelp)
    {
	usage();
	exit(1);
    }



    ####################################
    # Checking other parameters
    ####################################
    my $error = 0;
    if(    ($sheetDirname ne "" and $sheetFilename ne "") 
	or ($sheetDirname ne "" and -b $storableFilename ne "")
	or ($sheetFilename ne "" and -b $storableFilename ne ""))
    {
	print "\nError: The -s, -d and -b arguments can't be specified at the same time.\n";
	$error = 1;
    }
    elsif($sheetDirname eq "" and $sheetFilename eq "" and $storableFilename eq "")
    {
	print "\n\nError: You have to specify either the -d or -s or -b argument.\n";
	$error = 1;
    }

    if(   ($sheetDirnameVariant ne "" and $sheetFilenameVariant ne "")
       or ($sheetDirnameVariant ne "" and $storableFilenameVariant ne "")
       or ($storableFilenameVariant ne "" and $sheetFilenameVariant ne "")
	    )
    {
	print "\nError: The -vs and -vd  and -vb arguments can't be specified at the same time.\n";
	$error = 1;
    }

    if($platRel eq "")
    {
	print "\nError: You have to specify -r parameter.\n";
	$error = 1;
    }
    
    if($error)
    {
	usage();
	exit(1);
    }
    elsif($showHelp)
    {
	usage();
    }

    return ($platId,$platRel,$varName,$sheetDirname,$sheetFilename,$storableFilename,
	    $resultDirname,$varNameVariant,
	    $sheetDirnameVariant,$sheetFilenameVariant,$storableFilenameVariant,$formatNbr,
	    $addDefCaps,$rfsDir,$noIni);
}

###########################################################################
#
# The format of the returned hash is:
# NOTE: Still one bug, onge a range/key  has been defined it can't be removed 
#  {
#  <uid> => 
#    {
#    "uid_specific"   
#           = > {
#               "R" => (<capa1ForReading>,<capa2ForReading>)
#               "W" => (<capa1ForWriting>,<capa2ForWriting>)
#               "RSid" => (<sidForReading>)
#               "WSid" => (<sidForWriting>)
#               "owner"  => <someHexValue>
#               "rfs"  => <0/1>
#               "backup"  => <0/1>
#               }
#    "ranges"   
#           = > {
#            <fromRange>
#              => {
#                <toRange>
#                 => {
#                    "R" => (<capa1ForReading>,<capa2ForReading>)
#                     "W" => (<capa1ForWriting>,<capa2ForWriting>)
#                     "RSid" => (<sidForReading>)
#                     "WSid" => (<sidForWriting>)
#                     "rfs"  => <0/1>
#                     "backup"  => <0/1>       
#                     }
#                  }        
#               }
#    "keys" => {
#              <keyInt> 
#                   => {
#                      "read-only" => <0/1>
#                      "key type" => <bin/int/str/bool>
#                      "R" => (<capa1ForReading>,<capa2ForReading>)
#                      "W" => (<capa1ForWriting>,<capa2ForWriting>)
#                      "RSid" => (<sidForReading>)
#                      "WSid" => (<sidForWriting>)
#                      "value" => <value>
#                      "rfs"  => <0/1>
#                      "backup"  => <0/1>
#                       }
#                } # keys
###########################################################################
sub generateConfig
{
    my $refToResHash = shift;     # The generated configuration
    my $reftoConfigHash = shift;  # from where the configuration is generated from
    my $platId = shift;
    my $platRel = shift;
    my $varName = shift;

    if($varName =~ /^\s*$/)
    {
	# same as in cenrep_keys.pm
	$varName = "default";
    }

    # See syntax of reftoConfigHash from the documentation of
    # readCentrepKeyInfo
    my $refToHash;
    foreach my $uid (keys %$reftoConfigHash)
    {
	my $tmpUidResHash = addHash($uid,$refToResHash);
	my $refToUidConfigHash = $$reftoConfigHash{$uid};

	##########################################
	# Adding UID information
	##########################################
	my $refToUidSpecificResHash = addHash("uid_specific",$tmpUidResHash);

	my $refToUidSpecificConfigHash = $$refToUidConfigHash{"uid_specific"};
	my $refToUidSpecificVariantConfigHash = undef;

	#--------------------------------------------------
	# now check, if there is variant specific configuration
	#--------------------------------------------------
	if(defined($$refToUidSpecificConfigHash{$varName}))
	{
	    $refToUidSpecificVariantConfigHash 
		= getConfig($platId,$platRel,
			    $$refToUidSpecificConfigHash{$varName});
	}
	#--------------------------------------------------
	# if not found, then try to get default configuration
	#--------------------------------------------------
	if(!defined($refToUidSpecificVariantConfigHash) && $varName ne "default")
	{
	    $refToUidSpecificVariantConfigHash 
		= getConfig($platId,$platRel,
			    $$refToUidSpecificConfigHash{"default"});	    
	}
	#--------------------------------------------------
	# if we found configuration then add it to the hash
	#--------------------------------------------------
	if(defined($refToUidSpecificVariantConfigHash))
	{
	    $$refToUidSpecificResHash{"rfs"} = $$refToUidSpecificVariantConfigHash{"rfs"};
	    $$refToUidSpecificResHash{"backup"} = $$refToUidSpecificVariantConfigHash{"backup"};
	    $$refToUidSpecificResHash{"R"} = $$refToUidSpecificVariantConfigHash{"R"};
	    $$refToUidSpecificResHash{"W"} = $$refToUidSpecificVariantConfigHash{"W"};
	    $$refToUidSpecificResHash{"RSid"} = $$refToUidSpecificVariantConfigHash{"RSid"};
	    $$refToUidSpecificResHash{"WSid"} = $$refToUidSpecificVariantConfigHash{"WSid"};
	    $$refToUidSpecificResHash{"owner"} = $$refToUidSpecificVariantConfigHash{"owner"};
	}	    
	##########################################
	# Adding range information
	##########################################
	my $refToRangesResHash = addHash("ranges",$tmpUidResHash);

	my $refToRangesConfigHash = $$refToUidConfigHash{"ranges"};
	foreach my $rangeFrom (keys %$refToRangesConfigHash)
	{
	    my $refToRangeCommonConfigHash = ${$$refToRangesConfigHash{$rangeFrom}}{"common"};
	    # 1st check, if the key belongs to this release
	    if(isRemoved($platId,$platRel,$$refToRangeCommonConfigHash{"removed"}))
	    {
		next;
	    }

	    my $refToRangesToConfigHash = ${$$refToRangesConfigHash{$rangeFrom}}{"to"};
	    foreach my $rangeTo (keys %$refToRangesToConfigHash)
	    {
		my $refToSingleRangeToConfigHash 
		    = $$refToRangesToConfigHash{$rangeTo};
		my $refToRangeToVariantConfigHash = undef;
		#--------------------------------------------------
		# now check, if there is variant specific configuration
		#--------------------------------------------------
		if(defined($$refToSingleRangeToConfigHash{$varName}))
		{
		    $refToRangeToVariantConfigHash
			= getConfig($platId,$platRel,
				    $$refToSingleRangeToConfigHash{$varName});
		}
		#--------------------------------------------------
		# if not found, then try to get default configuration
		#--------------------------------------------------
		if(!defined($refToRangeToVariantConfigHash) && $varName ne "default")
		{
		    $refToRangeToVariantConfigHash
			= getConfig($platId,$platRel,
				    $$refToSingleRangeToConfigHash{"default"});	    
		}
		#--------------------------------------------------
		# if we found configuration then add it to the hash
		#--------------------------------------------------
		if(defined($refToRangeToVariantConfigHash))
		{
		    my $refRangeVariantResHash  = addHash($rangeFrom,$refToRangesResHash);
		    $refRangeVariantResHash  = addHash($rangeTo,$refRangeVariantResHash);
		    $$refRangeVariantResHash{"rfs"} = $$refToRangeToVariantConfigHash{"rfs"};
		    $$refRangeVariantResHash{"backup"} = $$refToRangeToVariantConfigHash{"backup"};
		    $$refRangeVariantResHash{"R"} = $$refToRangeToVariantConfigHash{"R"};
		    $$refRangeVariantResHash{"W"} = $$refToRangeToVariantConfigHash{"W"};
		    $$refRangeVariantResHash{"RSid"} = $$refToRangeToVariantConfigHash{"RSid"};
		    $$refRangeVariantResHash{"WSid"} = $$refToRangeToVariantConfigHash{"WSid"};
		}	
	    }
	}    
	##########################################
	# Adding key values
	##########################################
	my $tmpKeysResHash = addHash("keys",$tmpUidResHash);

	$refToHash = $$reftoConfigHash{$uid};
	$refToHash = $$refToHash{"keys"};
	foreach my $key (keys %$refToHash)
	{
	    my $refToCommonHash = ${$$refToHash{$key}}{"common"};
	    # 1st check, if the key belongs to this release
	    if(isRemoved($platId,$platRel,$$refToCommonHash{"removed"}))
	    {
		next;
	    }
	    #--------------------------------------------------
	    # now check, if there is variant specific configuration
	    #--------------------------------------------------
	    my $refToRelHash = ${$$refToHash{$key}}{"release"};
	    my $refToKeyConfigHash;
	    if(defined($$refToRelHash{$varName}))
	    {
		$refToKeyConfigHash = getConfig($platId,$platRel,
						$$refToRelHash{$varName});
	    }
	    #--------------------------------------------------
	    # if not found, then try to get default configuration
	    #--------------------------------------------------
	    if(!defined($refToKeyConfigHash) && $varName ne "default")
	    {
		$refToKeyConfigHash 
		    = getConfig($platId,$platRel,
				$$refToRelHash{"default"});	    
	    }
	    #--------------------------------------------------
	    # if we found configuration then add it to the hash
	    #--------------------------------------------------
	    if(defined($refToKeyConfigHash))
	    {
		my $keyResHash = addHash($key,$tmpKeysResHash);
		$$keyResHash{"read-only"} = $$refToCommonHash{"read-only"};
		$$keyResHash{"key type"} = $$refToCommonHash{"key type"};
		$$keyResHash{"value"} = $$refToKeyConfigHash{"value"};
		$$keyResHash{"rfs"} = $$refToKeyConfigHash{"rfs"};
		$$keyResHash{"backup"} = $$refToKeyConfigHash{"backup"};
		$$keyResHash{"R"} = $$refToKeyConfigHash{"R"};
		$$keyResHash{"W"} = $$refToKeyConfigHash{"W"};
		$$keyResHash{"RSid"} = $$refToKeyConfigHash{"RSid"};
		$$keyResHash{"WSid"} = $$refToKeyConfigHash{"WSid"};
	    }	    
	}
    }
}

###########################################################################
# reads either default config or variant config into the result hash.
# Note that the variant hash can't delete any keys, just override them.
###########################################################################
sub readSingleConfig
{
    my $refToExcel      = shift;
    my $sheetDirname    = shift;
    my $sheetFilename   = shift;
    my $storableFilename = shift;
    my $refToResultHash = shift;
    my $platId          = shift;
    my $platRel         = shift;
    my $varName         = shift;
    
    #-----------------------------------------------------------------
    # There are 2 options how the user can give the if-sheet names handled:
    # 1) He gives the directory where all files are located
    # 2) He gives an excel sheet, which contains all the files in column A.
    #-----------------------------------------------------------------
    my $refToExcelFilesList;

    if($sheetDirname !~ /^\s*$/)
    {
	# Read the filenames from the directory given
	$refToExcelFilesList = getExcelFilesFromDir $sheetDirname;
    }
    elsif($sheetFilename !~ /^\s*$/)
    {
	# Read the filenames from the excel file given
	$refToExcelFilesList = getExcelFilesFromFile $sheetFilename;
    }
    elsif($storableFilename  =~ /^\s*$/)
    {
	# nothing to do, just exit function
	return;
    }
    #-----------------------------------------------------------------
    # The main function that parses the information from sheets
    #-----------------------------------------------------------------
    my $refToDefHash;
    if($storableFilename  !~ /^\s*$/)
    {
        # read mem sheets info
	open(STORABLE_IN ,$storableFilename) || die "Unable to open: $storableFilename";
	$refToDefHash = fd_retrieve(\*STORABLE_IN);
	close STORABLE_IN;
    }
    else
    {
	$refToDefHash = readConfig($refToExcelFilesList,$refToExcel);
    }
#For testing to get the generated config printed to terminal
#     $Data::Dumper::Indent = 1;
#     print Dumper($refToDefHash);
#     exit 1;

    #-----------------------------------------------------------------
    # Determinate the default configuration 
    #-----------------------------------------------------------------
    generateConfig($refToResultHash,$refToDefHash,$platId,$platRel,$varName);
    $refToDefHash = undef;

#      #For testing to get the generated config printed to terminal
#      $Data::Dumper::Indent = 1;
#      print Dumper($refToResultHash);
#      exit 1;

}

###################################################
# Actual program
###################################################
my($platId,$platRel,$varName,$sheetDirname,$sheetFilename,$storableFilename,$resultDirname,
   $varNameVariant,$sheetDirnameVariant,$storableFilenameVariant,$sheetFilenameVariant,$formatNbr,
   $addDefCaps,$rfsDir,$noIni) = parseCmdLine;

# ----------------------------------------------------------------  
# Get excel application
# ----------------------------------------------------------------  
my $refToExcel = getExcelApp();

#-----------------------------------------------------------------
# read the default configuration hash
#-----------------------------------------------------------------
my %resultHash;
readSingleConfig($refToExcel,$sheetDirname,$sheetFilename,$storableFilename,\%resultHash,
		 $platId,$platRel,$varName);

#-----------------------------------------------------------------
# read the overriding variant  configuration hash
#-----------------------------------------------------------------
readSingleConfig($refToExcel,$sheetDirnameVariant,$sheetFilenameVariant,$storableFilenameVariant,
		 \%resultHash,$platId,$platRel,$varNameVariant);

#-----------------------------------------------------------------
# quit excel and "delete" the instances.
#-----------------------------------------------------------------
quitExcelApp($refToExcel);

#-----------------------------------------------------------------
# Generate the ini-files
#-----------------------------------------------------------------
if(! $noIni)
{
    generateIniFiles(\%resultHash,$resultDirname,$formatNbr,$addDefCaps);
}

#-----------------------------------------------------------------
# Generate the RFS file (if wanted)
#-----------------------------------------------------------------
if($rfsDir !~ /^\s*$/)
{
    $rfsDir =~ s/[\/\\]$//g;
    $rfsDir .= "/cenrep_rfs.txt";  
    generateRfsFile(\%resultHash,$rfsDir);
}

