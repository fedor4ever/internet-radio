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
#####################################
# Run with -h to get help
######################################
use strict;
use lib   ".";
use Win32::OLE qw(in with);
use Win32::OLE::Const 'Microsoft Excel';
use Cwd;
use Getopt::Long;
use File::Copy;
use excel_support;  # Own perl module that contains the reading of cenrep key-sheets
use cenrep_keys;  # Own perl module that contains the reading of cenrep key-sheets
use Storable qw(nstore_fd);
use Data::Dumper;


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
	$refToWorkSheet = getWorkSheet( $refToWb,"Key values");
	readCentrepKeyInfo( $refToWorkSheet,\%reshHash,undef,undef,$file);

#  	for( my $i = 0; $i < $$refToWb->Worksheets->Count; $i++)
#  	{
#  	    my $workSheet = $$refToWb->WorkSheets($i + 1);
#  	    $refToWorkSheet = getWorkSheet( $refToWb,$workSheet->Name);
#  	    readCentrepKeyInfo( $refToWorkSheet,\%reshHash);
#  	}
	# "delete" the workbooks, this closes the excel application
	$$refToWb->Close(0);
	undef $$refToWb;
	undef $refToWb;
    }

#    $Data::Dumper::Indent = 1;
#     print Dumper(%reshHash);
#      exit 1;

    return \%reshHash;
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
    print "This script is intended to be used to generate ini-files for Central Repository\n";
    print "based on the information in the cenrep key excel-sheet(s). These sheets define\n";
    print "the content of the ini-files.\n";
    print "\n";
    print "Rough discruption on how the values for the ini-files are determinated:\n";
    print "1) Content of sheets identified by -d or -s are read.\n";
    print "2) From sheets read in step 1 the information is parsed based on the\n";
    print "   information specified by -p, -r and -v\n";
    print "3) [optional] Default values specified by the steps 1 and 2 can be\n";
    print "    overriden by defining information in other sheets, specified by parameters\n";
    print "    -vd,-vs and -vv\n";
    print "\n";
    print "\n";
    print "Usage:\n";
    print "  $filename [-h|-help]\n";
    print "                (-d <dirName> | -s <sourceFilename>)\n";
    print "                [ -f <resultFile>]\n";
    print "\n";
    print "Options:\n";
    print " -h                  : Show this help\n";
    print " -help               : Show this help\n";
    print " -f <resultFile>     : The storable file name.\n";
    print " -d <dirName>        : The dir, which contains the sheets, which are read. Relative path to the\n";
    print "                       current dir or absoluth path. NOTE: the dir name can't contain any spaces.\n";
    print " -s <sourceFilename>       : The text filename, which contains sheet name with absoluth\n";
    print "                             path names (1/row)\n";
    print "                              (see rough description above) Optional.\n";
    print "\n";
    print "\n";
    print "You have specify either -d or -s, but not both at the same time.\n";
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
    my $sheetDirname = "";
    my $sheetFilename = "";
    my $resultFilename = "";
    my $showHelp = 0;

    if( ! GetOptions('d:s'      => \$sheetDirname,
		     's:s'      => \$sheetFilename,
		     'f:s'      => \$resultFilename,
		     'h'        => \$showHelp,
		     'help'     => \$showHelp,
		     '<>'       => \&usage))
    {
	usage();
	exit(1);
    }
    
    my $error = 0;
    if($sheetDirname ne "" and $sheetFilename ne "")
    {
	print "\nError: The -s and -d arguments can't be specified at the same time.\n";
	$error = 1;
    }
    elsif($sheetDirname eq "" and $sheetFilename eq "")
    {
	print "\n\nError: You have to specify either the -d or -s argument.\n";
	$error = 1;
    }

    if($resultFilename eq "")
    {
	print "\nError: You have to specify -rf parameter.\n";
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
    
    return ($sheetDirname,$sheetFilename,$resultFilename);
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
    else
    {
	# nothing to do, just exit function
	return;
    }
    #-----------------------------------------------------------------
    # The main function that parses the information from sheets
    #-----------------------------------------------------------------

    return readConfig($refToExcelFilesList,$refToExcel);
}

###################################################
# Actual program
###################################################
my($sheetDirname,$sheetFilename,$resultFilename) = parseCmdLine;

# ----------------------------------------------------------------  
# Get excel application
# ----------------------------------------------------------------  
my $refToExcel = getExcelApp();

#-----------------------------------------------------------------
# read the default configuration hash
#-----------------------------------------------------------------
my $refToResultHash = readSingleConfig($refToExcel,$sheetDirname,$sheetFilename);

#-----------------------------------------------------------------
# quit excel and "delete" the instances.
#-----------------------------------------------------------------
quitExcelApp($refToExcel);


open OUT, ">$resultFilename" or die "Unable to open $resultFilename: $!";
binmode OUT;
nstore_fd $refToResultHash, \*OUT;
close OUT;

