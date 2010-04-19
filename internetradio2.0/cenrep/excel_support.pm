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
###################################################
###################################################
# This module contains common helper functions for 
# handling excel sheets from perl.
###################################################
###################################################
package excel_support;

##########################################
# Libraries used
##########################################
use strict;
use Win32::OLE qw(in with);
use Win32::OLE::Const 'Microsoft Excel';
use Exporter ();     # Needed to make constants available to caller.

##########################################
# Defining all the "global" variables, that we are using.
##########################################
use vars qw(
            @ISA
            @EXPORT
            );


@ISA = qw(Exporter); # Inherit from Exporter module (import method needed).

# Define all the functions defined by this module
use subs qw(
	    getExcelApp
	    quitExcelApp
	    openWorkbook
	    getWorkSheet
	    getIndexByColName
	    );

##########################################
# Define the exports from this module that can be used
# Outside
##########################################
@EXPORT = qw(
             &getExcelApp
	     &quitExcelApp
	     &openWorkbook
	     &getWorkSheet
	     &getIndexByColName
             );


###########################################################################
# Function that creates an excel application and returns a reference to it.
#
# Params: -
#
# Return value: a reference to a excel application
###########################################################################
sub getExcelApp
{
    # get already active Excel application or open new
    my $excel = Win32::OLE->GetActiveObject('Excel.Application') || 
      Win32::OLE->new('Excel.Application', 'Quit');
    
    return \$excel;									    
}

###########################################################################
# Function that deletes an excel application given
#
# Params: refToExcelApp
#
# Return value: a reference to a excel application
###########################################################################
sub quitExcelApp
{
    my $refToExcelApp = shift;

    $$refToExcelApp->Quit();
    # For some reason this needs to be done twice inorder to close the excel.
    undef $$refToExcelApp;
    undef $refToExcelApp;
}

###########################################################################
# Function opens the given filename as workbook
#
# Params: Filename, referenceToExcelApp
#
# Return: reference to a workbook object 
###########################################################################
sub openWorkbook
{
    my $filename = shift;
    my $refToExcel = shift;

    # Open the workbook as read only
    my $workBook = ${$refToExcel}->Workbooks->Open( { FileName => $filename, ReadOnly=> xlReadOnly } );
    die "$filename: cannot open file\n" unless (defined $workBook );

    return \$workBook;
}

###########################################################################
# Function finds the wanted worksheet from the given workbook
#
# Params: referenceToWorkbook,wantedSheetName
#
# Return: reference to a worksheet object 
###########################################################################
sub getWorkSheet
{
    my $refToWb = shift;
    my $wantedSheetName = shift;
    
    my $workSheet;

    # Loop through all the workbooks and find a matching name.
    for( my $i = 0; $i < $$refToWb->WorkSheets->Count; $i++)
    {
	# The index is from 1-> and the counter starts from 0 => +1.
	$workSheet = $$refToWb->WorkSheets($i + 1);
        if( $workSheet->Name eq $wantedSheetName ) 
        {
	    # match found, return reference to the found worksheet
	    return \$workSheet;
	}
    }

    # No match found, error, thus return undef.
    return undef;
}           


###########################################################################
# Function that finds the index of the column based on given name
#
# Params: referenceToWorksheet,
#         wantedColName   (== the wanted column name)
#         maxcolCount     (== how many columns are checked for the given name) 
#
# Return: index to Column, 
#         -1, if no match found
###########################################################################
sub getIndexByColName
{
    my $refToWorksheet = shift;
    my $colName = shift;
    my $maxColCount = shift;
    
    my %i;
    # loop through columns to find the index, which 
    # matches to the given column name.
    for( my $i = 1; $i < $maxColCount; $i++ )
    {
	# The headers are always in row 1.
	my $colNameFromSheet = $$refToWorksheet->Cells( 1, $i  )->{ 'Value' };

	# check the name against the given name. If same then return the index.
        if($colName eq $colNameFromSheet)
	{
	    return $i;
	}	       
    }

    # No match found, return -1 as an error.
    return -1;
}


1;
