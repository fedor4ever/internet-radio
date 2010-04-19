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
use File::Basename;
use excel_support;  # Own perl module that contains the reading of memory-sheets
use cenrep_keys;  # Own perl module that contains the reading of memory-sheets

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
    my $refToExcel = shift;
    
    my @fileList;
    # ----------------------------------------------------------------  
    # Open the file and get the worksheet
    # ----------------------------------------------------------------  
    my $refToWb =  openWorkbook( $file,$refToExcel);
    my $refToWorkSheet = getWorkSheet( $refToWb,"Info");
    
    #------------------------------------------------------------
    # Read the needed rows from the row, until empty row found
    #------------------------------------------------------------
    my $row;
    for( $row = 1;; $row++ ) 
    {	         
	my $fileName = $$refToWorkSheet->Cells( $row, 1  )->{ 'Value' };
	
	if(!defined($fileName) || $fileName =~ /^\s*$/)
	{
	    last;
	}

	push(@fileList,$fileName);
    }
	
    return \@fileList;
}

###########################################################################
# Main function, 
###########################################################################
sub main
{
    my $refToExcelFiles = shift;
    my $refToExcel = shift;
    my $ignoreWarnings = shift;

    my %reshHash;
    my $refToWorkSheet;
    
    my $errorsFound = 0;

    # Loop through each found excel sheet. This will open one file after each other
    # and parse information into the hashes defined above.
    foreach my $file ( @{$refToExcelFiles} ) 
    {
	$file =~ /[\\\/]([a-zA-Z0-9\-_\.\s\&]+\.xls)/;
	print "\n--------------------------------------------\nReading: $1\n";

	my ($name,$path,$suffix) = fileparse($file,qr{\.xls});
	checkCentrepKeySheetName("$name$suffix");
	# ----------------------------------------------------------------  
	# Open the file
	# ----------------------------------------------------------------  
	my $refToWb = openWorkbook( $file,$refToExcel);
	
	# ----------------------------------------------------------------  
	# Read the information from the worksheets
	# ----------------------------------------------------------------
	for( my $i = 0; $i < $$refToWb->Worksheets->Count; $i++)
	{
	    my $workSheet = $$refToWb->WorkSheets($i + 1);
	    $refToWorkSheet = getWorkSheet( $refToWb,$workSheet->Name);
	    if(checkCentrepKeyInfo( $refToWorkSheet,$ignoreWarnings))
	    {
		$errorsFound = 1;
	    }
	}
	# "delete" the workbooks, this closes the excel application	    
	$$refToWb->Close(0);
	undef $$refToWb;
	undef $refToWb;
    }

    return $errorsFound;
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
    print "\n";
    print "\n";
    print "This script reads all the cenrep keys sheets from the given\n";
    print "directory and makes a syntax check for them.\n";
    print "\n";
    print "Usage:\n";
    print "  $0 [-h|-help] (-d <cenrepSheetDir> | -f <excelSheetName>) \n";
    print "\n";
    print "Options:\n";
    print "  -h                  : Show this help\n";
    print "  -help               : Show this help\n";
    print "  -d <sheetDir>       : relative path from the dir where this script is run\n";
    print "                        to the directory where the cenrep-sheets exist.\n";
    print "  -e <excelSheetName> : name of the sheet, that specifies the read excel sheet names in the column A.\n";
    print "                        No empty rows allowed, reading is stopped, when the 1st empty row is found.\n";	
    print "                        The sheet name defining the files inside Workbook has to be \"Info\".\n";	
    print "                        Also no header expected for the column (reading starts from row 1)\n";
    print "  -w                  : Ignores warning messages\n";
    print "  -h                  : Show this help\n";
    print "\n";
    print "You have specify either -d or -e, but not both at the same time.\n";
    print "\n";
    print "Example usage:\n";
    print "    perl $0  -d cenrep\n"; 
    print "\n";
    print "Known issues:\n";
    print "   - The script will give an error, if directory names in the path contain spaces\n";
    print "\n";
    print "\n";

    exit(0);
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
    my $dir = "";
    my $file = "";
    my $sheetFile = "";
    my $ignoreWarnings = 0;
    
    if( ! GetOptions('d:s'      => \$dir,
		     's:s'      => \$sheetFile,
		     'f:s'      => \$file,
		     'h'        => \&usage,
		     'w'        => \$ignoreWarnings,
		     'help'     => \&usage,
		     '<>'       => \&usage))
    {
	exit(1);
    }
    
    if($dir ne "" and $file ne "" )
    {
	print "\nError: The file and directory arguments can't be specified at the same time.\n";
	exit(1);
    }
    elsif($dir eq "" and $file eq "" )
    {
	usage;
	print "\n\nError: You have to specify either the  -d or -f argument.\n";
	exit(1);
    }
    
    
    return ($dir,$file,$sheetFile,$ignoreWarnings);
}


###################################################
# Actual program
###################################################
my $sheetDir;
my $file;
my $sheetFile;
my $ignoreWarnings;

($sheetDir,$file,$sheetFile,$ignoreWarnings) = parseCmdLine;

# ----------------------------------------------------------------  
# Get excel application
# ----------------------------------------------------------------  
my $refToExcel = getExcelApp();

#-----------------------------------------------------------------
# There are 2 options how the user can give the if-sheet names handled:
# 1) He gives the directory where all files are located
# 2) He gives an excel sheet, which contains all the files in column A.
#-----------------------------------------------------------------
my $refToExcelFilesList;

if($sheetFile !~ /^\s*$/)
{
    my @list;
    push(@list,$sheetFile);
    $refToExcelFilesList =  \@list;
}
if($sheetDir !~ /^\s*$/)
{
    # Read the filenames from the directory given
    $refToExcelFilesList = getExcelFilesFromDir $sheetDir;
}
else
{
    # Read the filenames from the excel file given
     $refToExcelFilesList = getExcelFilesFromFile $file, $refToExcel;
}

#-----------------------------------------------------------------
# The main function that parses the information
#-----------------------------------------------------------------

my $errors = main $refToExcelFilesList,$refToExcel,$ignoreWarnings;

#-----------------------------------------------------------------
# quit excel and "delete" the instances.
#-----------------------------------------------------------------
quitExcelApp($refToExcel);

exit $errors;











