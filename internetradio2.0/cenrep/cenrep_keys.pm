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
# Version 0.9
#    - now checking that Int and Real values are really 
#      numbers
#    - rfs, cust config and backup are must fields for variant row
#    - All rows, which have "Comment:" in the uid name are ignored.
###################################################
###################################################
package cenrep_keys;

##########################################
# Libraries used
##########################################
use strict;
use Win32::OLE qw(in with);
use Win32::OLE::Const 'Microsoft Excel';
use Exporter ();     # Needed to make constants available to caller.
use excel_support;    # opening/closing workbook, etc.. functions.

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
	    hasValue
	    addHash
	    readCol
	    readCentrepKeyInfo
	    checkCentrepKeyInfo
	    checkCentrepKeySheetName
	    );

##########################################
# Define the exports from this module that can be used
# Outside
##########################################
@EXPORT = qw(
	     &addHash
	     &readCol
             &readCentrepKeyInfo
	     &checkCentrepKeyInfo
	     &checkCentrepKeySheetName
             );


my $globalMaxColCount = 43;
my $globalMaxEmptyRowCount = 10;

#######################################
# Checks that the cell exists and has some value
#
# Params: refToWorkSheet,row,column
#
# Return: 1 if the cell had some value
#         0 if the cell did not have a value
#######################################
sub hasValue
{
    my $refToWorksheet = shift;
    my $row = shift;
    my $col = shift;

    if($col < 1)
    {
	return 0;
    }
    if(defined($$refToWorksheet->Cells( $row, $col  )->{ 'Value' }))
    {
	return 1;
    }
    if(!(defined($$refToWorksheet->Cells( $row, $col  ))  ||$$refToWorksheet->Cells( $row, $col  )->{ 'Value' }))
    {
	print "$row $col\n";
	print $$refToWorksheet->Name,"\n";
    }
    if($$refToWorksheet->Cells( $row, $col  )->{ 'Value' } !~ /^\s*$/)
    {
	return 1;
    }

    return 0;
}

#######################################
# Reads a cell value from given worksheet and given place.
# If the value is empty then initializes teh return value to the
# given default value
#
# Params: refToWorkSheet,row,column,defValue
#
# Return: the read value
#######################################
sub readCol
{
    my $refToWorksheet = shift;
    my $row = shift;
    my $col = shift;
    my $defValue = shift;

    my $retVal = undef;
    if($col > 0)
    {
	$retVal = $$refToWorksheet->Cells( $row, $col  )->{ 'Value' };
    }
    if(!defined($retVal) || $retVal =~ /^\s*$/)
    {
	$retVal = $defValue;
    }
    
    return $retVal;
}
#######################################
# If there is no given key defined in the given hash, then
# adds a key to point to a new hash
#
# Params: newKey,refToHash,refToHash
#
# Return: ref to added hash, or existing hash
#######################################
sub addHash
{
    my $key = shift;
    my $refToHash = shift;
    
    if(! defined( $$refToHash{$key}))
    {
	my %newHash;
	$$refToHash{$key} = \%newHash;
    }

    return $$refToHash{$key};
}

###########################################################################
#
###########################################################################
sub addCapability
{
    my $refToRList = shift;
    my $refToWList = shift;
    my $capaColumnValue = shift;
    my $capaName = shift;

    if($capaColumnValue =~ /R/i)
    {
	push(@$refToRList,$capaName);
    }
    if($capaColumnValue =~ /W/i)
    {
	push(@$refToWList,$capaName);
    }
}

###########################################################################
#
###########################################################################
sub readIndexes
{
   my $refToWorksheet = shift;

   my @retList;
    
   push(@retList,getIndexByColName($refToWorksheet,"Uid Name",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Uid Value",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Range From",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Range To",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Key Name",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Key Int",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Shared Data Name",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Removed",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Platform Release",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Variant Name",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Def Value",$globalMaxColCount));
   my $index = getIndexByColName($refToWorksheet,"Read Only",$globalMaxColCount);
   if($index  < 0)
   {
       $index = getIndexByColName($refToWorksheet,"Read  Only",$globalMaxColCount);
   }
   push(@retList,$index);
   push(@retList,getIndexByColName($refToWorksheet,"Key Type",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Backup",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"RFS",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Cust Config",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"None",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"All",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"All - TCB",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"NetworkServices",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"LocalServices",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"ReadUserData",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"WriteUserData",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Location",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"ReadDeviceData",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"WriteDeviceData",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"ProtServ",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"DRM",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"SwEvent",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"PowerMgmt",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"AllFiles",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"DiskAdmin",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"NetworkControl",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"MultiMediaDD",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"CommDD",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"TCB",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"TrustedUI",$globalMaxColCount));

   push(@retList,getIndexByColName($refToWorksheet,"AlwaysPass",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"UserEnvironment",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"SurroundingsDD",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"SID Read",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"SID Write",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Range From",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Range To",$globalMaxColCount));
   push(@retList,getIndexByColName($refToWorksheet,"Owner",$globalMaxColCount));


   return @retList;
}

###########################################################################
# Function that reads all columns from the central repository sheet to the 
# given hash.
# The format of the returned hash is:
#  {
#  <uid> => 
#    {
#    "name" => <uidName>
#    "uid_specific"  
#           = > {
#               <variant>
#                   => {
#                      <release> 
#                          => { 
#                              "R" => (<capa1ForReading>,<capa2ForReading>)
#                              "W" => (<capa1ForWriting>,<capa2ForWriting>)
#                              "RSid" => (<sidForReading>)
#                              "WSid" => (<sidForWriting>)
#                              "owner" => <ownerSid>
#                              "rfs"  => <0/1>
#                              "backup"  => <0/1>
#                              }
#                      }
#               }
#    "ranges" 
#           => {
#              <rangeFrom> 
#                => {                  
#                   "common" => (   # == defined in range row
#                                "removed" => <release> [optional, only if specified]
#                                }
#                   "to" => {
#                          <rangeTo>
#                            => {
#                                <variant>
#                                    => {
#                                       <release> 
#                                            => { 
#                                               "R" => (<capa1ForReading>,<capa2ForReading>)
#                                               "W" => (<capa1ForWriting>,<capa2ForWriting>)
#                                               "RSid" => (<sidForReading>)
#                                               "WSid" => (<sidForWriting>)
#                                               "rfs"  => <0/1>
#                                               "backup"  => <0/1>
#                                               }
#                                        }
#                                }
#                             }   
#                      }
#                   }   
#              } # ranges end                
#    "keys" => {
#              <keyInt> 
#                   => {
#                      "common" => (   # == defined in key row
#                                  "name" => <keyName>
#                                  "sheetName" => <eexcelSheetName, from which this key was read>
#                                  "sd-name" => <sharedDataName> [optional, only if specified]
#                                  "read-only" => <0/1>
#                                  "key type" => <bin/int/str/bool>
#                                  "removed" => <release> [optional, only if specified]
#                                  "owner" => <ownerName>
#                                  "entity" => <ownerEntity>
#                                  }
#                      "release"
#                           => {
#                              <variant>  # "default" is special case
#                                 => {
#                                    <release> 
#                                        => { 
#                                           "R" => (<capa1ForReading>,<capa2ForReading>)
#                                           "W" => (<capa1ForWriting>,<capa2ForWriting>)
#                                           "RSid" => (<sidForReading>)
#                                           "WSid" => (<sidForWriting>)
#                                           "value" => <value>
#                                           "rfs"  => <0/1>
#                                           "backup"  => <0/1>
#                                            }
#                                    }
#                              }
#                       }
#                } # keys
#
#
#
#
# Params: referenceToWorksheet,refToHash
#
# Return: 0 on successfull reading
#         1 on case, where the worksheet was not OK (for isntance name is 
#                     not according to specification.
###########################################################################
sub readCentrepKeyInfo   
{
    my $refToWorksheet = shift;
    my $refToHash = shift;
    my $owner = shift;
    my $entity = shift;
    my $sheetName = shift;
   
    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------  
    # Uid value
    # Key name value
    # Key value
    # 
    # 
    # 
    # 
    # 
    
    my ($uidNameIndex,$uidValueIndex,$rangeFromIndex,$rangeToIndex,$keyNameValueIndex,$keyIntValueIndex,$sdNameIndex,$removedIndex,$releaseIndex,$variantNameIndex,$valueIndex,$readOnlyIndex,
	$keyTypeIndex,$backupIndex,$rfsIndex,$custConfigIndex,$capaNoneIndex,$capaAllIndex,$capaAllMinusTcbIndex,$capaNetworkServicesIndex,
	$capaLocalServicesIndex,$capaReadUserDataIndex,$capaWriteUserDataIndex,$capaLocationIndex,$capaReadDeviceDataIndex,
	$capaWriteDeviceDataIndex,$capaProtServIndex,$capaDrmIndex,$capaSwEventIndex,$capaPowerMgmtIndex,$capaAllFilesIndex,
	$capaDiskAdminIndex,$capaNetworkControlIndex,$capaMultiMediaDDIndex,$capaCommDDIndex,
	$capaTCBIndex,$capaTrustedUIIndex,$capaAlwaysPassIndex,$capaUserEnvIndex,$capaSurroundingsDDIndex,
	$capaSidReadIndex,$capaSidWriteIndex,$rangeFromIndex,$rangeToIndex,$ownerIndex) = readIndexes($refToWorksheet);

    if( $uidValueIndex < 0)
    {
	return;
    }

    my $row = 1;
    my $emptyRowCount = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    my $currUid = "";
    my $currUidKeysHash;
    my $currUidRangesHash;
    my $currUidSpecificHash;
    my $currRangeHash = undef;
    my $currKey = "";
    my $currKeyReleaseHash;  # the ref to has that is found from "release"
    my $lastActiveRowType = 0;   # 0 = not defined, 1 = uid, 2 = range, 3 = key

    for( $row = 2;; $row++ ) 
    {	         
	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (   hasValue($refToWorksheet,$row, $uidValueIndex )
	      || hasValue($refToWorksheet,$row, $keyIntValueIndex )	
	      || hasValue($refToWorksheet,$row, $valueIndex )	
	      || hasValue($refToWorksheet,$row, $releaseIndex )	
	      || hasValue($refToWorksheet,$row, $rangeFromIndex )	
	      || hasValue($refToWorksheet,$row, $rangeToIndex )	
	     ))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		last;
	    }
	    next;
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}
		
	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
	my $uidName             = readCol($refToWorksheet,$row,$uidNameIndex,"");
	my $uid                 = readCol($refToWorksheet,$row,$uidValueIndex,"");
	my $keyName             = readCol($refToWorksheet,$row,$keyNameValueIndex,"");
	my $key                 = readCol($refToWorksheet,$row,$keyIntValueIndex,"");
	my $sdName              = readCol($refToWorksheet,$row,$sdNameIndex,"");
	my $removed             = readCol($refToWorksheet,$row,$removedIndex,"");
	my $release             = readCol($refToWorksheet,$row,$releaseIndex,"");
	my $variant             = readCol($refToWorksheet,$row,$variantNameIndex,"");
	my $value               = readCol($refToWorksheet,$row,$valueIndex,"");
	my $ro                  = readCol($refToWorksheet,$row,$readOnlyIndex,"");
	my $keyType             = readCol($refToWorksheet,$row,$keyTypeIndex,"");
	my $backup              = readCol($refToWorksheet,$row,$backupIndex,"");
	my $rfs                 = readCol($refToWorksheet,$row,$rfsIndex,"");
	my $custConfig          = readCol($refToWorksheet,$row,$custConfigIndex,"");
	my $capaNone            = readCol($refToWorksheet,$row,$capaNoneIndex,"");         
	my $capaAll             = readCol($refToWorksheet,$row,$capaAllIndex,"");         
	my $capaAllMinusTcb     = readCol($refToWorksheet,$row,$capaAllMinusTcbIndex,"");
	my $capaNetworkServices = readCol($refToWorksheet,$row,$capaNetworkServicesIndex,"");
	my $capaLocalServices   = readCol($refToWorksheet,$row,$capaLocalServicesIndex,"");
	my $capaReadUserData    = readCol($refToWorksheet,$row,$capaReadUserDataIndex,"");  
	my $capaWriteUserData   = readCol($refToWorksheet,$row,$capaWriteUserDataIndex,"");
	my $capaLocation        = readCol($refToWorksheet,$row,$capaLocationIndex,"");  
	my $capaReadDeviceData  = readCol($refToWorksheet,$row,$capaReadDeviceDataIndex,"");
	my $capaWriteDeviceData = readCol($refToWorksheet,$row,$capaWriteDeviceDataIndex,"");
	my $capaProtServ        = readCol($refToWorksheet,$row,$capaProtServIndex,"");
	my $capaDrm             = readCol($refToWorksheet,$row,$capaDrmIndex,"");
	my $capaSwEvent         = readCol($refToWorksheet,$row,$capaSwEventIndex,"");
	my $capaPowerMgmt       = readCol($refToWorksheet,$row,$capaPowerMgmtIndex,"");
	my $capaAllFiles        = readCol($refToWorksheet,$row,$capaAllFilesIndex,"");
	my $capaDiskAdmin       = readCol($refToWorksheet,$row,$capaDiskAdminIndex,"");
	my $capaNetworkControl  = readCol($refToWorksheet,$row,$capaNetworkControlIndex,"");
	my $capaMultiMediaDD    = readCol($refToWorksheet,$row,$capaMultiMediaDDIndex,"");
	my $capaCommDD          = readCol($refToWorksheet,$row,$capaCommDDIndex,"");
	my $capaTCB             = readCol($refToWorksheet,$row,$capaTCBIndex,"");
	my $capaTrustedUI       = readCol($refToWorksheet,$row,$capaTrustedUIIndex,"");

	my $capaAlwaysPass      = readCol($refToWorksheet,$row,$capaAlwaysPassIndex,"");
	my $capaUserEnv         = readCol($refToWorksheet,$row,$capaUserEnvIndex,"");
	my $capaSurroundingsDD  = readCol($refToWorksheet,$row,$capaSurroundingsDDIndex,"");
	my $capaSidRead         = readCol($refToWorksheet,$row,$capaSidReadIndex,"");
	my $capaSidWrite        = readCol($refToWorksheet,$row,$capaSidWriteIndex,"");
	my $rangeFrom           = readCol($refToWorksheet,$row,$rangeFromIndex,"");
	my $rangeTo             = readCol($refToWorksheet,$row,$rangeToIndex,"");
	my $uidOwner            = readCol($refToWorksheet,$row,$ownerIndex,"");

	if($uidName =~ /^\s*Comment:/i)
	{
	    # comment rows are ignored
	    next;
	}

	#------------------------------------------------------------
	# Handle UID row
	#------------------------------------------------------------
	if($uid !~ /^\s*$/)
	{
	    $currUid = $uid;
	    $currKey = "";
	    $lastActiveRowType = 1;
	    my $tmpRefToHash = addHash($uid,$refToHash);
	    $currUidKeysHash = addHash("keys",$tmpRefToHash);
	    $$tmpRefToHash{"name"} = $uidName;
	    $currUidRangesHash = addHash("ranges",$tmpRefToHash);
	    $currUidSpecificHash = addHash("uid_specific",$tmpRefToHash);
	}
	#------------------------------------------------------------
	# Handle range row
	#------------------------------------------------------------
	elsif($rangeFrom !~ /^\s*$/ || $rangeTo !~ /^\s*$/)
	{
	    $currRangeHash = addHash($rangeFrom,$currUidRangesHash);
	    my $commonHash = addHash("common",$currRangeHash);
	    $currRangeHash = addHash("to",$currRangeHash);
	    $currRangeHash = addHash($rangeTo,$currRangeHash);

	    $$commonHash{"removed"} = $removed;
	    $lastActiveRowType = 2;
	}
	#------------------------------------------------------------
	# Handle key row
	#------------------------------------------------------------
	elsif($key !~ /^\s*$/)
	{
	    $currKey = $key;
	    $lastActiveRowType = 3;
	    my $keyHash = addHash($key,$currUidKeysHash);
	    ######################################################
            #   "common" => (
	    #               "name" => <someName>
	    #               "sheetName" => <someNameWithoutPath>
	    #               "sd-name" => <someName>  [optional]
            #               "read-only" => <0/1>
            #               "key type" => <bin/int/str/bool>
	    #               "removed" => <release>
	    #               "owner" => <ownerName>
	    #               "entity" => <entity>
            #               }
	    ######################################################
	    my $commonHash = addHash("common",$keyHash);

	    $$commonHash{"owner"} = $owner;
	    $$commonHash{"entity"} = $entity;
	    $$commonHash{"name"} = $keyName;
	    $$commonHash{"sheetName"} = $sheetName;
	    
            # read-only attribute is set always from the "key row". 
	    # It can't be overwritten
	    $$commonHash{"read-only"} = 0;
	    if($ro =~ /^\s*yes\s*$/i)
	    {
		$$commonHash{"read-only"} = 1;
	    }
	    # key type attribute is set always from the "key row". It can't be overwritten
	    $$commonHash{"key type"} = $keyType;

	    # removed [ optional, we only defined, if specified]
	    if($removed !~ /^\s*$/)
	    {
		$$commonHash{"removed"} = $removed;
	    }

	    # shared data name [ optional, we only defined, if specified]
	    if($sdName !~ /^\s*$/)
	    {
		$$commonHash{"sd-name"} = $sdName;
	    }
	    
	    ######################################################
            #   "release" => {
	    #                ....
            #                }     
	    # information to this has in filled later on.
	    ######################################################
	    $currKeyReleaseHash = addHash("release",$keyHash);	    
	}
	#------------------------------------------------------------
	# Handle variant row
	#------------------------------------------------------------
	elsif($release !~ /^\s*$/)
	{
	    # harmonize the release syntax
	    if($variant =~ /^\s*$/)
	    {
		$variant = "default";
	    }
	    my $relHashRef;
	    if($lastActiveRowType == 3)
	    {
		$relHashRef = addHash($variant,$currKeyReleaseHash);
	    }
	    elsif($lastActiveRowType == 2)
	    {
		$relHashRef = addHash($variant,$currRangeHash);
	    }
	    elsif($lastActiveRowType == 1)
	    {
		$relHashRef = addHash($variant,$currUidSpecificHash);
	    }
	    ######################################################
            #  <release>
            #      => { 
            #         "R" => (<capa1ForReading>,<capa2ForReading>)
            #         "W" => (<capa1ForWriting>,<capa2ForWriting>)
            #         "RSid" => (<sidForReading>)
            #         "WSid" => (<sidForWriting>)
            #         "value" => <value>             # only if this is for keys
            #         "owner" => <uid>               # only if this is for uid
            #         "rfs"  => <0/1>
            #         "backup"  => <0/1>
            #         }
	    ######################################################
	    $release =~ /s([0-9]+)\s+([0-9]+\.[0-9]+)/i;
	    my $usedRel = "S$1 $2";
	    my $refToVarHash = addHash($usedRel,$relHashRef);
	    # Value and owner are specific only to some variant row types.
	    if($lastActiveRowType == 3)   # This variant row is for a key
	    {
		$$refToVarHash{"value"} = $value;
	    }
	    elsif($lastActiveRowType == 1) # This variant row is for the uid
	    {
		$$refToVarHash{"owner"} = $uidOwner;
	    }
	    $$refToVarHash{"rfs"} = 0;
	    if($rfs =~ /^\s*yes\s*$/i)
	    {
		$$refToVarHash{"rfs"} = 1;
	    }
	    $$refToVarHash{"backup"} = 0;
	    if($backup =~ /^\s*yes\s*$/i)
	    {
		$$refToVarHash{"backup"} = 1;
	    }
	    if(!defined($$refToVarHash{"W"}))
	    {
		my @list;
		$$refToVarHash{"W"} = \@list;
	    }
	    if(!defined($$refToVarHash{"R"}))
	    {
		my @list;
		$$refToVarHash{"R"} = \@list;
	    }

	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaNone,"AlwaysPass");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaAlwaysPass,"AlwaysPass");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaNetworkServices,"NetworkServices");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaLocalServices,"Local Services");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaReadUserData,"ReadUserData");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaWriteUserData,"WriteUserData");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaLocation,"Location");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaReadDeviceData,"ReadDeviceData");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaWriteDeviceData,"WriteDeviceData");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaProtServ,"ProtServ");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaDrm,"DRM");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaSwEvent,"SwEvent");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaPowerMgmt,"PowerMgmt");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaAllFiles,"AllFiles");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaDiskAdmin,"DiskAdmin");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaNetworkControl,"NetworkControl");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaMultiMediaDD,"MultiMediaDD");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaCommDD,"CommDD");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaTCB,"TCB");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaTrustedUI,"TrustedUI");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaUserEnv,"UserEnvironment");
	    addCapability($$refToVarHash{"R"},$$refToVarHash{"W"},$capaSurroundingsDD,"SurroundingsDD");
	    
	    my @list1;
	    $$refToVarHash{"RSid"} = \@list1;
	    if($capaSidRead !~ /^\s*$/)
	    {
		push(@list1,$capaSidRead);
	    }
	    my @list2;
	    $$refToVarHash{"WSid"} = \@list2;
	    if($capaSidWrite !~ /^\s*$/)
	    {
		push(@list2,$capaSidWrite);
	    }
	}
    }

    return $refToHash;
}


###########################################################################
#
###########################################################################
sub checkRelease
{
    my $row = shift;
    my $value = shift;
    my $columnName = shift;

    if($value !~ /^\s*S60\s+[1-9]+\.[0-9]+\s*$/ && $value !~ /^\s*$/)
    {
	print "Row $row: $columnName -column has wrong release syntax. (should be for instance \"S60 3.0\"\n";
	return 1;
    }
    return 0;
}

###########################################################################
#
###########################################################################
sub checkCapability
{
    my $row = shift;
    my $value = shift;
    my $columnName = shift;
    
    if($value !~ /^\s*$/ &&
      $value !~ /^R$/ &&
      $value !~ /^W$/ &&
      $value !~ /^RW$/ 
      )
    {
	print "Row $row: The $columnName -column has invalid value \"$value\".\n";
	return 1;
    }    
    return 0;
}

###########################################################################
# Makes syntax checks to the given worksheet
#
#
# Params: referenceToWorksheet
#
###########################################################################
sub checkCentrepKeyInfo   
{
    my $refToWorksheet = shift;
    my $ignoreWarnings = shift;

    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------      
    my ($uidNameIndex,$uidValueIndex,$rangeFromIndex,$rangeToIndex,
	$keyNameValueIndex,$keyIntValueIndex,$sdNameIndex,$removedIndex,$releaseIndex,$variantNameIndex,
	$valueIndex,$readOnlyIndex,
	$keyTypeIndex,$backupIndex,$rfsIndex,$custConfigIndex,$capaNoneIndex,$capaAllIndex,$capaAllMinusTcbIndex,$capaNetworkServicesIndex,
	$capaLocalServicesIndex,$capaReadUserDataIndex,$capaWriteUserDataIndex,$capaLocationIndex,$capaReadDeviceDataIndex,
	$capaWriteDeviceDataIndex,$capaProtServIndex,$capaDrmIndex,$capaSwEventIndex,$capaPowerMgmtIndex,$capaAllFilesIndex,
	$capaDiskAdminIndex,$capaNetworkControlIndex,$capaMultiMediaDDIndex,$capaCommDDIndex,
	$capaTCBIndex,$capaTrustedUIIndex,$capaAlwaysPassIndex,$capaUserEnvIndex,$capaSurroundingsDDIndex,
	$capaSidReadIndex,$capaSidWriteIndex,$rangeFromIndex,$rangeToIndex,$ownerIndex) = readIndexes($refToWorksheet);
    if( $uidValueIndex < 0)
    {
	return;
    }

    my $row = 1;
    my $emptyRowCount = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    my $currUid = "";
    my $currRange = "";
    my $lastActiveRowType = 0;   # 0 = not defined, 1 = uid, 2 = range, 3 = key
    my $variantRowCount = 0;
    my $errorCount = 0;
    my $currentKeyType = "";
    my $currentKeyReadOnly = "";
    for( $row = 2;; $row++ ) 
    {	         
	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (   hasValue($refToWorksheet,$row, $uidValueIndex )
	      || hasValue($refToWorksheet,$row, $uidNameIndex )
	      || hasValue($refToWorksheet,$row, $keyNameValueIndex )	
	      || hasValue($refToWorksheet,$row, $keyIntValueIndex )	
	      || hasValue($refToWorksheet,$row, $releaseIndex )	
	      || hasValue($refToWorksheet,$row, $rangeFromIndex )	
	      || hasValue($refToWorksheet,$row, $rangeToIndex )	
	     ))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		if($variantRowCount == 0)
		{
		    if($lastActiveRowType == 1)
		    {
			print "Row $row: There were no variants defined for previous UID.\n";
			print "          This means among other things that default access is alwaysFail.\n";	    
			$errorCount++;
		    }
		    elsif($lastActiveRowType == 2)
		    {
			print "Row $row: There were no variants defined for previous range => range will not appear in ini-file.\n";
			$errorCount++;
		    }
		    elsif($lastActiveRowType == 3)
		    {
			print "Row $row: There were no variants defined for previous key => key will not appear in ini-file.\n";
			$errorCount++;
		    }
		}
		last;
	    }
	    next;
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}
		
	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
	my $uidName             = readCol($refToWorksheet,$row,$uidNameIndex,"");
	my $uid                 = readCol($refToWorksheet,$row,$uidValueIndex,"");
	my $rangeFrom           = readCol($refToWorksheet,$row,$rangeFromIndex,"");
	my $rangeTo             = readCol($refToWorksheet,$row,$rangeToIndex,"");
	my $keyName             = readCol($refToWorksheet,$row,$keyNameValueIndex,"");
	my $key                 = readCol($refToWorksheet,$row,$keyIntValueIndex,"");
	my $removed             = readCol($refToWorksheet,$row,$removedIndex,"");
	my $release             = readCol($refToWorksheet,$row,$releaseIndex,"");
	my $variant             = readCol($refToWorksheet,$row,$variantNameIndex,"");
	my $value               = readCol($refToWorksheet,$row,$valueIndex,"");
	my $ro                  = readCol($refToWorksheet,$row,$readOnlyIndex,"");
	my $keyType             = readCol($refToWorksheet,$row,$keyTypeIndex,"");
	my $backup              = readCol($refToWorksheet,$row,$backupIndex,"");
	my $rfs                 = readCol($refToWorksheet,$row,$rfsIndex,"");
	my $custConfig          = readCol($refToWorksheet,$row,$custConfigIndex,"");         
	my $capaNone            = readCol($refToWorksheet,$row,$capaNoneIndex,"");         
	my $capaAll             = readCol($refToWorksheet,$row,$capaAllIndex,"");         
	my $capaAllMinusTcb     = readCol($refToWorksheet,$row,$capaAllMinusTcbIndex,"");
	my $capaNetworkServices = readCol($refToWorksheet,$row,$capaNetworkServicesIndex,"");
	my $capaLocalServices   = readCol($refToWorksheet,$row,$capaLocalServicesIndex,"");
	my $capaReadUserData    = readCol($refToWorksheet,$row,$capaReadUserDataIndex,"");  
	my $capaWriteUserData   = readCol($refToWorksheet,$row,$capaWriteUserDataIndex,"");
	my $capaLocation        = readCol($refToWorksheet,$row,$capaLocationIndex,"");  
	my $capaReadDeviceData  = readCol($refToWorksheet,$row,$capaReadDeviceDataIndex,"");
	my $capaWriteDeviceData = readCol($refToWorksheet,$row,$capaWriteDeviceDataIndex,"");
	my $capaProtServ        = readCol($refToWorksheet,$row,$capaProtServIndex,"");
	my $capaDrm             = readCol($refToWorksheet,$row,$capaDrmIndex,"");
	my $capaSwEvent         = readCol($refToWorksheet,$row,$capaSwEventIndex,"");
	my $capaPowerMgmt       = readCol($refToWorksheet,$row,$capaPowerMgmtIndex,"");
	my $capaAllFiles        = readCol($refToWorksheet,$row,$capaAllFilesIndex,"");
	my $capaDiskAdmin       = readCol($refToWorksheet,$row,$capaDiskAdminIndex,"");
	my $capaNetworkControl  = readCol($refToWorksheet,$row,$capaNetworkControlIndex,"");
	my $capaMultiMediaDD    = readCol($refToWorksheet,$row,$capaMultiMediaDDIndex,"");
	my $capaCommDD          = readCol($refToWorksheet,$row,$capaCommDDIndex,"");
	my $capaTCB             = readCol($refToWorksheet,$row,$capaTCBIndex,"");
	my $capaTrustedUI       = readCol($refToWorksheet,$row,$capaTrustedUIIndex,"");

	my $capaAlwaysPass      = readCol($refToWorksheet,$row,$capaAlwaysPassIndex,"");
	my $capaUserEnv         = readCol($refToWorksheet,$row,$capaUserEnvIndex,"");
	my $capaSurroundingsDD  = readCol($refToWorksheet,$row,$capaSurroundingsDDIndex,"");
	my $capaSidRead         = readCol($refToWorksheet,$row,$capaSidReadIndex,"");
	my $capaSidWrite        = readCol($refToWorksheet,$row,$capaSidWriteIndex,"");
	my $rangeFrom           = readCol($refToWorksheet,$row,$rangeFromIndex,"");
	my $rangeTo             = readCol($refToWorksheet,$row,$rangeToIndex,"");
	my $uidOwner            = readCol($refToWorksheet,$row,$ownerIndex,"");

	if($uidName =~ /^\s*Comment:/)
	{
	    # comment rows are ignored
	    print "Warning: Row $row ignored, since it is a comment row\n" if not $ignoreWarnings;
	    next;
	}

	#------------------------------------------------------------
	# Common checks
	#------------------------------------------------------------
	if($uidName !~ /^\s*$/ && $uid =~ /^\s*$/)
	{
	    # both uid and uidname have to be defined in the same row
	    print "Row $row: Uid Name-column has value, but not the Uid Value-column\n";
	    $errorCount++;
	}
	if($keyName !~ /^\s*$/ && $key =~ /^\s*$/)
	{
	    # both key int and keyname have to be defined in the same row
	    print "Row $row: Key Name-column has value, but not the Key Int-column\n";
	    $errorCount++;
	}
	if($rangeTo !~ /^\s*$/ && $rangeFrom =~ /^\s*$/)
	{
	    # if range to is defined, then also range from has to be defined.
	    print "Row $row: Range To-column has value, but not the Range From-column\n";
	    $errorCount++;
	}
	if($rangeTo =~ /^\s*$/ && $rangeFrom !~ /^\s*$/)
	{
	    # if range from is defined, then also range to  has to be defined.
	    print "Row $row: Range From-column has value, but not the Range To-column\n";
	    $errorCount++;
	}
	#------------------------------------------------------------
	# Handle UID row
	#------------------------------------------------------------
	if($uid !~ /^\s*$/)
	{
	    $currUid = $uid;
	    # Make sure that last uid/key/range had some variant rows
	    if($variantRowCount == 0)
	    {
		if($lastActiveRowType == 1)
		{
		    print "Row $row: There were no variants defined for previous UID.\n";
		    print "          This means among other things that default access is alwaysFail.\n";	    
		    $errorCount++;
		}
		elsif($lastActiveRowType == 2)
		{
		    print "Row $row: There were no variants defined for previous range => range will not appear in ini-file.\n";
		    $errorCount++;
		}
		elsif($lastActiveRowType == 3)
		{
		    print "Row $row: There were no variants defined for previous key => key will not appear in ini-file.\n";
		    $errorCount++;
		}
	    }
	    $lastActiveRowType = 1;
	    $variantRowCount = 0;
	    #######################################
            # Check: In the uid row the only allowed
	    #          columns are "uid name", "uid value", "purpose"
	    #######################################
	    if($rangeFrom !~ /^\s*$/)
	    {
		print "Row $row: Range From-column is not allowed for uid row\n";
		$errorCount++;
	    }	    
	    if($rangeTo !~ /^\s*$/)
	    {
		print "Row $row: Range To-column is not allowed for uid row\n";
		$errorCount++;
	    }	    
	    if($uidOwner !~ /^\s*$/)
	    {
		print "Row $row: Owner-column is not allowed for uid row\n";
		$errorCount++;
	    }	    
	    if($key !~ /^\s*$/)
	    {
		print "Row $row: Key Int-column is not allowed for uid row\n";
		$errorCount++;
	    }	    
	    if($value !~ /^\s*$/)
	    {
		print "Row $row: Def value-column is not allowed for uid row\n";
		$errorCount++;
	    }
	    if($rfs !~ /^\s*$/)
	    {
		print "Row $row: RFS-column is not allowed for uid row\n";
		$errorCount++;
	    }
	    if($ro !~ /^\s*$/)
	    {
		print "Row $row: Read Only-column is not allowed for uid row\n";
		$errorCount++;
	    }
	    if($keyType !~ /^\s*$/)
	    {
		print "Row $row: Key Type-column is not allowed for uid row\n";
		$errorCount++;
	    }
	    if($backup !~ /^\s*$/)
	    {
		print "Row $row: Backup-column is not allowed for uid row\n";
		$errorCount++;
	    }
	    if($custConfig !~ /^\s*$/)
	    {
		print "Row $row: Cust Config-column is not allowed for uid row\n";
		$errorCount++;
	    }
	    #######################################
            # Check: Check that uid is in hex format 
	    #######################################
	    if($uid != /^\s*0x/)
	    {
		print "Row $row: The Uid value has to be a hex value (starting with 0x)\n";
		$errorCount++;
	    }
	    elsif(length($uid) != 10)
	    {
		print "Row $row: The Uid length is not correct. Are you sure that the Uid is correct ?\n";
		$errorCount++;
	    }
	    #######################################
            # Check: Check that no capas defined
	    #######################################
	    if( $capaNone            !~ /^\s*$/ ||
		$capaAll             !~ /^\s*$/ ||
		$capaAllMinusTcb     !~ /^\s*$/ ||
		$capaNetworkServices !~ /^\s*$/ ||
		$capaLocalServices   !~ /^\s*$/ ||
		$capaReadUserData    !~ /^\s*$/ ||
		$capaWriteUserData   !~ /^\s*$/ ||
		$capaLocation        !~ /^\s*$/ ||
		$capaReadDeviceData  !~ /^\s*$/ ||
		$capaWriteDeviceData !~ /^\s*$/ ||
		$capaProtServ        !~ /^\s*$/ ||
		$capaDrm             !~ /^\s*$/ ||
		$capaSwEvent         !~ /^\s*$/ ||
		$capaPowerMgmt       !~ /^\s*$/ ||
		$capaAllFiles        !~ /^\s*$/ ||
		$capaDiskAdmin       !~ /^\s*$/ ||
		$capaNetworkControl  !~ /^\s*$/ ||
		$capaMultiMediaDD    !~ /^\s*$/ ||
		$capaCommDD          !~ /^\s*$/ ||
		$capaTCB             !~ /^\s*$/ ||
		$capaTrustedUI       !~ /^\s*$/ ||
		$capaAlwaysPass      !~ /^\s*$/ ||    
		$capaUserEnv         !~ /^\s*$/ ||       
		$capaSurroundingsDD  !~ /^\s*$/ ||
		$capaSidRead         !~ /^\s*$/ ||
		$capaSidWrite        !~ /^\s*$/
		)
	    {
		print "Row $row: Capabilities are not allowed to be defined in UID row.\n";
		    $errorCount++;
	    }
	}
	#------------------------------------------------------------
	# Handle range row
	#------------------------------------------------------------
	elsif($rangeFrom !~ /^\s*$/)
	{
	    # Make sure that last uid/key/range had some variant rows
	    if($variantRowCount == 0)
	    {
		if($lastActiveRowType == 1)
		{
		    print "Row $row: There were no variants defined for previous UID.\n";
		    print "          This means among other things that default access is alwaysFail.\n";	    
		    $errorCount++;
		}
		elsif($lastActiveRowType == 2)
		{
		    print "Row $row: There were no variants defined for previous range => range will not appear in ini-file.\n";
		    $errorCount++;
		}
		elsif($lastActiveRowType == 3)
		{
		    print "Row $row: There were no variants defined for previous key => key will not appear in ini-file.\n";
		    $errorCount++;
		}
	    }
	    $variantRowCount = 0;
	    $lastActiveRowType = 2;

	    if($rangeTo =~ /^\s*$/)
	    {
		print "Row $row: You have to specify both Range To and Range From in the range row.\n";
		$errorCount++;
	    }	    
	    #######################################
            # Check: There has to be uid defined before any ranges 
	    #######################################
	    if($currUid =~ /^\s*$/)
	    {
		print "Row $row: Before any ranges are defined, an UID has to be defined\n";
		$errorCount++;
	    }

	    #######################################
            # Check: In the range row the only allowed
	    #          columns are "range from", "range to", "purpose", "removed"
	    #######################################
	    if($uid !~ /^\s*$/)
	    {
		print "Row $row: Uid-column is not allowed for range row\n";
		$errorCount++;
	    }	    
	    if($uidName !~ /^\s*$/)
	    {
		print "Row $row: Uid Name-column is not allowed for uid row\n";
		$errorCount++;
	    }	    
	    if($uidOwner !~ /^\s*$/)
	    {
		print "Row $row: Owner-column is not allowed for range row\n";
		$errorCount++;
	    }	    
	    if($key !~ /^\s*$/)
	    {
		print "Row $row: Key Int-column is not allowed for range row\n";
		$errorCount++;
	    }	    
	    if($value !~ /^\s*$/)
	    {
		print "Row $row: Def value-column is not allowed for range row\n";
		$errorCount++;
	    }
	    if($rfs !~ /^\s*$/)
	    {
		print "Row $row: RFS-column is not allowed for range row\n";
		$errorCount++;
	    }
	    if($ro !~ /^\s*$/)
	    {
		print "Row $row: Read Only-column is not allowed for range row\n";
		$errorCount++;
	    }
	    if($keyType !~ /^\s*$/)
	    {
		print "Row $row: Key Type-column is not allowed for range row\n";
		$errorCount++;
	    }
	    if($backup !~ /^\s*$/)
	    {
		print "Row $row: Backup-column is not allowed for range row\n";
		$errorCount++;
	    }
	    if($custConfig !~ /^\s*$/)
	    {
		print "Row $row: Cust Config-column is not allowed for range row\n";
		$errorCount++;
	    }
	    #######################################
            # Check: Check that range from and range to are in hex format 
	    #######################################
	    if($rangeFrom != /^\s*0x/)
	    {
		print "Row $row: The range from value has to be a hex value (starting with 0x)\n";
		$errorCount++;
	    }
	    elsif(length($rangeFrom) != 10)
	    {
		print "Row $row: The range from-length is not correct. Are you sure that the range is correct ?\n";
		$errorCount++;
	    }
	    if($rangeTo != /^\s*0x/)
	    {
		print "Row $row: The range to- value has to be a hex value (starting with 0x)\n";
		$errorCount++;
	    }
	    elsif(length($rangeTo) != 10)
	    {
		print "Row $row: The range to-length is not correct. Are you sure that the range is correct ?\n";
		$errorCount++;
	    }
	    #######################################
            # Check: Check that no capas defined
	    #######################################
	    if( $capaNone            !~ /^\s*$/ ||
		$capaAll             !~ /^\s*$/ ||
		$capaAllMinusTcb     !~ /^\s*$/ ||
		$capaNetworkServices !~ /^\s*$/ ||
		$capaLocalServices   !~ /^\s*$/ ||
		$capaReadUserData    !~ /^\s*$/ ||
		$capaWriteUserData   !~ /^\s*$/ ||
		$capaLocation        !~ /^\s*$/ ||
		$capaReadDeviceData  !~ /^\s*$/ ||
		$capaWriteDeviceData !~ /^\s*$/ ||
		$capaProtServ        !~ /^\s*$/ ||
		$capaDrm             !~ /^\s*$/ ||
		$capaSwEvent         !~ /^\s*$/ ||
		$capaPowerMgmt       !~ /^\s*$/ ||
		$capaAllFiles        !~ /^\s*$/ ||
		$capaDiskAdmin       !~ /^\s*$/ ||
		$capaNetworkControl  !~ /^\s*$/ ||
		$capaMultiMediaDD    !~ /^\s*$/ ||
		$capaCommDD          !~ /^\s*$/ ||
		$capaTCB             !~ /^\s*$/ ||
		$capaTrustedUI       !~ /^\s*$/ ||
		$capaAlwaysPass      !~ /^\s*$/ ||    
		$capaUserEnv         !~ /^\s*$/ ||       
		$capaSurroundingsDD  !~ /^\s*$/ ||
		$capaSidRead         !~ /^\s*$/ ||
		$capaSidWrite        !~ /^\s*$/
		)
	    {
		print "Row $row: Capabilities are not allowed to be defined in range row.\n";
		    $errorCount++;
	    }
	}
	#------------------------------------------------------------
	# Handle key row
	#------------------------------------------------------------
	elsif($key !~ /^\s*$/)
	{
	    # Make sure that last uid/key/range had some variant rows
	    if($variantRowCount == 0)
	    {
		if($lastActiveRowType == 1)
		{
		    print "Row $row: There were no variants defined for previous UID.\n";
		    print "          This means among other things that default access is alwaysFail.\n";	    
		    $errorCount++;
		}
		elsif($lastActiveRowType == 2)
		{
		    print "Row $row: There were no variants defined for previous range => range will not appear in ini-file.\n";
		    $errorCount++;
		}
		elsif($lastActiveRowType == 3)
		{
		    print "Row $row: There were no variants defined for previous key => key will not appear in ini-file.\n";
		    $errorCount++;
		}
	    }
	    $variantRowCount = 0;
	    $lastActiveRowType = 3;

	    $currentKeyType = $keyType;
	    # Store the ro value into the variable (we need this to handle the key variant rows
	    $currentKeyReadOnly = $ro;

	    #######################################
            # Check: There has to be uid defined before any keys 
	    #######################################
	    if($currUid =~ /^\s*$/)
	    {
		print "Row $row: Before any keys are defined an UID has to be defined\n";
		$errorCount++;
	    }

	    #######################################
            # Check: Check that key int is in hex format 
	    #######################################
	    if($key !~ /^\s*0x/)
	    {
		print "Row $row: The Key Int  has to be a hex value (starting with 0x)\n";
		$errorCount++;
	    }
	    elsif(length($key) != 10)
	    {
		print "Row $row: The Key int length is not correct. Are you sure that the hex value is correct ?\n";
		$errorCount++;
	    }

	    #######################################
            # Check: Check that key type and read only are defined
	    #######################################
	    if($keyType =~  /^\s*$/)
	    {
		print "Row $row: The Key Type-column has to be defined in row, where key is defined.\n";
		$errorCount++;
	    }
	    if($ro =~  /^\s*$/)
	    {
		print "Row $row: The Read Only-column has to be defined in row, where key is defined.\n";
		$errorCount++;
	    }

	    #######################################
            # Check: key type column
	    #######################################
 	    if($keyType !~ /^Int$/ &&
	       $keyType !~ /^Real$/ &&
	       $keyType !~ /^Str$/ &&
	       $keyType !~ /^Bin$/
	       )
	    {
		print "Row $row: The Key type-column has invalid value.\n";
		$errorCount++;
	    }

	    #######################################
            # Check: Removed column syntax
	    #######################################
	    $errorCount += checkRelease($row,$removed,"Removed");

	    #######################################
            # Check: columns that should not be in key row
	    #######################################
	    if($rangeFrom !~ /^\s*$/)
	    {
		print "Row $row: Range From-column is not allowed for key row\n";
		$errorCount++;
	    }	    
	    if($rangeTo !~ /^\s*$/)
	    {
		print "Row $row: Range To-column is not allowed for key row\n";
		$errorCount++;
	    }	    
	    if($uidOwner !~ /^\s*$/)
	    {
		print "Row $row: Owner-column is not allowed for key row\n";
		$errorCount++;
	    }	    
	    if($release !~ /^\s*$/)
	    {
		print "Row $row: Platform Release-column is not allowed for key row\n";
		$errorCount++;
	    }
	    if($value !~ /^\s*$/)
	    {
		print "Row $row: Def value-column is not allowed for key row\n";
		$errorCount++;
	    }
	    if($rfs !~ /^\s*$/)
	    {
		print "Row $row: RFS-column is not allowed for key row\n";
		$errorCount++;
	    }
	    if($backup !~ /^\s*$/)
	    {
		print "Row $row: Backup-column is not allowed for key row\n";
		$errorCount++;
	    }
	    if($custConfig !~ /^\s*$/)
	    {
		print "Row $row: Cust Config-column is not allowed for key row\n";
		$errorCount++;
	    }
	    #######################################
            # Check: Check that no capas defined
	    #######################################
	    if( $capaNone            !~ /^\s*$/ ||
		$capaAll             !~ /^\s*$/ ||
		$capaAllMinusTcb     !~ /^\s*$/ ||
		$capaNetworkServices !~ /^\s*$/ ||
		$capaLocalServices   !~ /^\s*$/ ||
		$capaReadUserData    !~ /^\s*$/ ||
		$capaWriteUserData   !~ /^\s*$/ ||
		$capaLocation        !~ /^\s*$/ ||
		$capaReadDeviceData  !~ /^\s*$/ ||
		$capaWriteDeviceData !~ /^\s*$/ ||
		$capaProtServ        !~ /^\s*$/ ||
		$capaDrm             !~ /^\s*$/ ||
		$capaSwEvent         !~ /^\s*$/ ||
		$capaPowerMgmt       !~ /^\s*$/ ||
		$capaAllFiles        !~ /^\s*$/ ||
		$capaDiskAdmin       !~ /^\s*$/ ||
		$capaNetworkControl  !~ /^\s*$/ ||
		$capaMultiMediaDD    !~ /^\s*$/ ||
		$capaCommDD          !~ /^\s*$/ ||
		$capaTCB             !~ /^\s*$/ ||
		$capaTrustedUI       !~ /^\s*$/ ||
		$capaAlwaysPass      !~ /^\s*$/ ||    
		$capaUserEnv         !~ /^\s*$/ ||       
		$capaSurroundingsDD  !~ /^\s*$/ ||
		$capaSidRead         !~ /^\s*$/ ||
		$capaSidWrite        !~ /^\s*$/
		)
	    {
		print "Row $row: Capabilities are not allowed to be defined in Key row.\n";
		    $errorCount++;
	    }
	}
	#------------------------------------------------------------
	# Handle variant row
	#------------------------------------------------------------
	elsif($release !~ /^\s*$/)
	{
	    $variantRowCount++;
	    #######################################
            # Check: Variant row should not contain removed, keyType or Read only column
	    #######################################
	    if($keyType !~  /^\s*$/)
	    {
		print "Row $row: The Key Type-column can't be defined in variant-row.\n";
		$errorCount++;
	    }
	    if($ro !~  /^\s*$/)
	    {
		print "Row $row: The Read Only-column can't be defined in variant-row.\n";
		$errorCount++;
	    }
	    if($removed !~  /^\s*$/)
	    {
		print "Row $row: The Removed-column can't be defined in variant-row.\n";
		$errorCount++;
	    }

	    #######################################
            # Check: Platform release column syntax
	    #######################################
	    $errorCount += checkRelease($row,$release,"Platform Release");

	    #######################################
            # Check: Owner value only allowed for uid variant row
	    #######################################
 	    if( $lastActiveRowType == 1)
	    {
		if($uidOwner =~ /^\s*$/)
		{
		    print "Row $row: No owner defined for the uid variant row.\n";
		    $errorCount++;		    
		}
	    }
 	    elsif( $lastActiveRowType != 1)
	    {
		if($uidOwner !~ /^\s*$/)
		{
		    print "Row $row: Owner-column is only valid for the uid variant row.\n";
		    $errorCount++;		    
		}
	    }
	    #######################################
            # Check: Variant name column
	    #######################################
	    if($variant !~ /^\s*$/ &&
	       $variant !~ /^elaf$/ &&
	       $variant !~ /^apac$/ &&
	       $variant !~ /^thai$/ &&
	       $variant !~ /^japan$/ &&
	       $variant !~ /^gsm$/ &&
	       $variant !~ /^low_cost$/
	       )
	    {
		print "Row $row: The Variant name-column has invalid value.\n";
		$errorCount++;
	    }

	    #######################################
            # Check: def value column
	    # (empty value only allowed in case of str and binary)
	    # Checks only valid, if the row is key's variant row.
	    # for other we check that the column is not filled.
	    #######################################
 	    if( $lastActiveRowType == 3)
	    {
		if($currentKeyType !~ /^Str$/ && $currentKeyType !~ /^Bin$/ && $value =~ /^\s*$/)
		{		
		    print "Row $row: The Def value-column has to have some value.\n";
		    $errorCount++;
		}
		if($currentKeyType =~ /^Int$/ && $value !~ /^\-*[0-9]+$/)
		{
		    print "Row $row: The Def value-column has to be a number, since type is Int.\n";
		    $errorCount++;
		}
		if($currentKeyType =~ /^Real$/ && $value !~ /^\-*[0-9\.]+$/)
		{
		    print "Row $row: The Def value-column has to be a number, since type is Real.\n";
		    $errorCount++;
		}
	    }
	    elsif($value !~ /^\s*$/)
	    {
		    print "Row $row: The Def value-column should not be defined for ";
		    if($lastActiveRowType ==1)
		    {
			print "Uid variant row\n";
		    }
		    elsif($lastActiveRowType ==2)
		    {
			print "range variant row\n";
		    }
		    $errorCount++;		
	    }

	    #######################################
            # Check: backup and rfs columns
	    #######################################
	    if($backup =~ /^\s*$/)
	    {
		print "Row $row: The Backup-column has to be specified in variant row.\n";
		$errorCount++;
	    }
	    elsif($backup !~ /^Yes$/ &&
		  $backup !~ /^No$/
		  )
	    {
		print "Row $row: The Backup-column has invalid value.\n";
		$errorCount++;
	    }
	    if($release !~ /^\s*$/ && $lastActiveRowType == 3 && $currentKeyReadOnly =~ /yes/i && $backup =~ /^Yes$/)
	    {
		print "Row $row: A readonly key should not be backed up.\n";
		$errorCount++;		
	    }

	    if($rfs =~ /^\s*$/)
	    {
		print "Row $row: The RFS-column has to have value in the variant row.\n";
		$errorCount++;
	    }
	    elsif($rfs !~ /^Yes$/ &&
		  $rfs !~ /^No$/
		  )
	    {
		print "Row $row: The RFS-column has invalid value.\n";
		$errorCount++;
	    }
	    if($release !~ /^\s*$/ && $lastActiveRowType == 3 && $currentKeyReadOnly =~ /yes/i && $rfs =~ /^Yes$/)
	    {
		print "Row $row: A readonly key should not have RFS as yes.\n";
		$errorCount++;		
	    }

	    #######################################
            # Check: customer config column
	    #    Only allowed for key variant row
	    #######################################
	    if( $lastActiveRowType == 3)
	    {		
		if($custConfig =~ /^\s*$/)
		{
		    print "Row $row: The Cust Config-column has to have value in the variant row.\n";
		    $errorCount++;
		}
		elsif($custConfig !~ /^Yes$/ &&
		      $custConfig !~ /^No$/ &&
		      $custConfig !~ /^Must$/
		      )
		{
		    print "Row $row: The Cust config-column has invalid value.\n";
		    $errorCount++;
		}
	    }
	    elsif($custConfig !~ /^\s*$/)
	    {
		    print "Row $row: The Cust Config-column should not be defined in ";
		    if($lastActiveRowType ==1)
		    {
			print "Uid variant row.\n";
		    }
		    elsif($lastActiveRowType ==2)
		    {
			print "range variant row.\n";
		    }
		    $errorCount++;		
	    }
	    #######################################
            # Check: at least one capability needs to be specified if release is specified
	    #######################################
	    if($release !~ /^\s*$/)
	    {
		if( $capaNone            =~ /^\s*$/ &&
		    $capaAll             =~ /^\s*$/ &&
		    $capaAllMinusTcb     =~ /^\s*$/ &&
		    $capaNetworkServices =~ /^\s*$/ &&
		    $capaLocalServices   =~ /^\s*$/ &&
		    $capaReadUserData    =~ /^\s*$/ &&
		    $capaWriteUserData   =~ /^\s*$/ &&
		    $capaLocation        =~ /^\s*$/ &&
		    $capaReadDeviceData  =~ /^\s*$/ &&
		    $capaWriteDeviceData =~ /^\s*$/ &&
		    $capaProtServ        =~ /^\s*$/ &&
		    $capaDrm             =~ /^\s*$/ &&
		    $capaSwEvent         =~ /^\s*$/ &&
		    $capaPowerMgmt       =~ /^\s*$/ &&
		    $capaAllFiles        =~ /^\s*$/ &&
		    $capaDiskAdmin       =~ /^\s*$/ &&
		    $capaNetworkControl  =~ /^\s*$/ &&
		    $capaMultiMediaDD    =~ /^\s*$/ &&
		    $capaCommDD          =~ /^\s*$/ &&
		    $capaTCB             =~ /^\s*$/ &&
		    $capaTrustedUI       =~ /^\s*$/ &&
		    $capaAlwaysPass      =~ /^\s*$/ &&    
		    $capaUserEnv         =~ /^\s*$/ &&       
		    $capaSurroundingsDD  =~ /^\s*$/ &&
		    $capaSidRead         =~ /^\s*$/ &&
		    $capaSidWrite        =~ /^\s*$/
		    )
		{
		    print "Row $row: At least some capability colums has to have some value, since Platform Release-column is defined.\n";
		    $errorCount++;
		}
	    }	   

	    #######################################
            # Check: if this is variant row for key, then if the key is read-only key, then
	    #        there should be no "W" defined
	    #######################################
	    if($release !~ /^\s*$/ && $lastActiveRowType == 3 && $currentKeyReadOnly =~ /yes/i)
	    {
		if( $capaNone            =~ /W/i ||
		    $capaAll             =~ /W/i ||
		    $capaAllMinusTcb     =~ /W/i ||
		    $capaNetworkServices =~ /W/i ||
		    $capaLocalServices   =~ /W/i ||
		    $capaReadUserData    =~ /W/i ||
		    $capaWriteUserData   =~ /W/i ||
		    $capaLocation        =~ /W/i ||
		    $capaReadDeviceData  =~ /W/i ||
		    $capaWriteDeviceData =~ /W/i ||
		    $capaProtServ        =~ /W/i ||
		    $capaDrm             =~ /W/i ||
		    $capaSwEvent         =~ /W/i ||
		    $capaPowerMgmt       =~ /W/i ||
		    $capaAllFiles        =~ /W/i ||
		    $capaDiskAdmin       =~ /W/i ||
		    $capaNetworkControl  =~ /W/i ||
		    $capaMultiMediaDD    =~ /W/i ||
		    $capaCommDD          =~ /W/i ||
		    $capaTCB             =~ /W/i ||
		    $capaTrustedUI       =~ /W/i ||
		    $capaAlwaysPass      =~ /W/i ||    
		    $capaUserEnv         =~ /W/i ||       
		    $capaSurroundingsDD  =~ /W/i ||
		    $capaSidRead         =~ /W/i ||
		    $capaSidWrite        =~ /W/i
		    )
		{
		    print "Row $row: Since the key is read-only, there should be no W-capa defined.\n";
		    $errorCount++;
		}
	    }	   
	    #######################################
            # Check: checking the usage of the "R"-capa.
	    #######################################
	    if($release !~ /^\s*$/ && $lastActiveRowType == 3)
	    {
		# You should not use R-capa with the below capabilities
		if( $capaWriteUserData   =~ /R/i ||
		    $capaWriteDeviceData =~ /R/i ||
		    $capaSidWrite        =~ /R/i
		    )
		{
		    print "Row $row: R-capa should not be used with association of Write-related capas.\n";
		    $errorCount++;
		}
		
		# In 99% cases you should allow R-to be always "alwayspass". Thus we report this as an error.
		if( $capaNone            =~ /R/i ||
		    $capaAll             =~ /R/i ||
		    $capaAllMinusTcb     =~ /R/i ||
		    $capaNetworkServices =~ /R/i ||
		    $capaLocalServices   =~ /R/i ||
		    $capaReadUserData    =~ /R/i ||
		    $capaWriteUserData   =~ /R/i ||
		    $capaLocation        =~ /R/i ||
		    $capaReadDeviceData  =~ /R/i ||
		    $capaWriteDeviceData =~ /R/i ||
		    $capaProtServ        =~ /R/i ||
		    $capaDrm             =~ /R/i ||
		    $capaSwEvent         =~ /R/i ||
		    $capaPowerMgmt       =~ /R/i ||
		    $capaAllFiles        =~ /R/i ||
		    $capaDiskAdmin       =~ /R/i ||
		    $capaNetworkControl  =~ /R/i ||
		    $capaMultiMediaDD    =~ /R/i ||
		    $capaCommDD          =~ /R/i ||
		    $capaTCB             =~ /R/i ||
		    $capaTrustedUI       =~ /R/i ||
		    $capaUserEnv         =~ /R/i ||       
		    $capaSurroundingsDD  =~ /R/i ||
		    $capaSidRead         =~ /R/i ||
		    $capaSidWrite        =~ /R/i
		    )
		{
		    print "Row $row: NOTE: only sensitive data (passwords and alike) should be protected for reading.\n";
		    print "          In normal case read should be allowed for everyone (== AlwaysPass-capa).\n";
			print "          Depending on the mening of key this is/is not an error (99% of cases this is an error).\n";
		    $errorCount++;
		}
	    }	   
	    #######################################
            # Check: capability colums
	    #######################################
	    $errorCount += checkCapability($row,$capaNone,"None");
	    $errorCount += checkCapability($row,$capaAll,"All");
	    $errorCount += checkCapability($row,$capaAllMinusTcb,"All - TCB");
	    $errorCount += checkCapability($row,$capaNetworkServices,"NetworkServices");
	    $errorCount += checkCapability($row,$capaLocalServices,"Local Services");
	    $errorCount += checkCapability($row,$capaReadUserData,"ReadUserData");
	    $errorCount += checkCapability($row,$capaWriteUserData,"WriteUserData");
	    $errorCount += checkCapability($row,$capaLocation,"Location");
	    $errorCount += checkCapability($row,$capaReadDeviceData,"ReadDeviceData");
	    $errorCount += checkCapability($row,$capaWriteDeviceData,"WriteDeviceData");
	    $errorCount += checkCapability($row,$capaProtServ,"ProtServ");
	    $errorCount += checkCapability($row,$capaDrm,"DRM");
	    $errorCount += checkCapability($row,$capaSwEvent,"SwEvent");
	    $errorCount += checkCapability($row,$capaPowerMgmt,"PowerMgmt");
	    $errorCount += checkCapability($row,$capaAllFiles,"AllFiles");
	    $errorCount += checkCapability($row,$capaDiskAdmin,"DiskAdmin");
	    $errorCount += checkCapability($row,$capaNetworkControl,"NetworkControl");
	    $errorCount += checkCapability($row,$capaMultiMediaDD,"MultiMediaDD");
	    $errorCount += checkCapability($row,$capaCommDD,"CommDD");
	    $errorCount += checkCapability($row,$capaTCB,"TCB");
	    $errorCount += checkCapability($row,$capaAlwaysPass,"AlwaysPass");
	    $errorCount += checkCapability($row,$capaUserEnv,"UserEnvironment");
	    $errorCount += checkCapability($row,$capaSurroundingsDD,"SurroundingsDD");
	    if($capaSidRead !~ /^\s*$/ && $capaSidRead !~ /^0x[0-9]{8}$/)
	    {
		print "Row $row: the Sid Read-column has invalid value. Only 1 sid is allowed in hex format.\n";
		$errorCount++;
	    }
	    if($capaSidWrite !~ /^\s*$/ && $capaSidWrite !~ /^0x[0-9]{8}$/)
	    {
		print "Row $row: the Sid Write-column has invalid value. Only 1 sid is allowed in hex format.\n";
		$errorCount++;
	    }

	    #######################################
            # Check: capability colums limitations
	    # max capability checks are:
	    #  7 capabilities
	    #  1 sid and 3 capabilities
	    #######################################
	    my @rList;
	    my @wList;
	    addCapability(\@rList,\@wList,$capaNone,"AlwaysPass");
	    addCapability(\@rList,\@wList,$capaAlwaysPass,"AlwaysPass");
	    addCapability(\@rList,\@wList,$capaNetworkServices,"NetworkServices");
	    addCapability(\@rList,\@wList,$capaLocalServices,"Local Services");
	    addCapability(\@rList,\@wList,$capaReadUserData,"ReadUserData");
	    addCapability(\@rList,\@wList,$capaWriteUserData,"WriteUserData");
	    addCapability(\@rList,\@wList,$capaLocation,"Location");
	    addCapability(\@rList,\@wList,$capaReadDeviceData,"ReadDeviceData");
	    addCapability(\@rList,\@wList,$capaWriteDeviceData,"WriteDeviceData");
	    addCapability(\@rList,\@wList,$capaProtServ,"ProtServ");
	    addCapability(\@rList,\@wList,$capaDrm,"DRM");
	    addCapability(\@rList,\@wList,$capaSwEvent,"SwEvent");
	    addCapability(\@rList,\@wList,$capaPowerMgmt,"PowerMgmt");
	    addCapability(\@rList,\@wList,$capaAllFiles,"AllFiles");
	    addCapability(\@rList,\@wList,$capaDiskAdmin,"DiskAdmin");
	    addCapability(\@rList,\@wList,$capaNetworkControl,"NetworkControl");
	    addCapability(\@rList,\@wList,$capaMultiMediaDD,"MultiMediaDD");
	    addCapability(\@rList,\@wList,$capaCommDD,"CommDD");
	    addCapability(\@rList,\@wList,$capaTCB,"TCB");
	    addCapability(\@rList,\@wList,$capaTrustedUI,"TrustedUI");
	    addCapability(\@rList,\@wList,$capaUserEnv,"UserEnvironment");
	    addCapability(\@rList,\@wList,$capaSurroundingsDD,"SurroundingsDD");

	    my $rSidDefined = 0;
	    if($capaSidRead !~ /^\s*$/)
	    {
		$rSidDefined = 4;
	    }
	    my $wSidDefined = 0;
	    if($capaSidWrite !~ /^\s*$/)
	    {
		$wSidDefined = 4;
	    }
	    
	    if((scalar(@rList) + $rSidDefined) > 7)
	    {
		print "Row $row: To many Read capability checks defined. Allowed max amounts are:\n";
		print "          (7 capabilities) or (1 sid and 3 capabilities)\n";  
		$errorCount++;
	    }
	    if((scalar(@wList) + $wSidDefined) > 7)
	    {
		print "Row $row: To many Write capability checks defined. Allowed max amounts are:\n";
		print "          (7 capabilities) or (1 sid and 3 capabilities)\n";  
		$errorCount++;
		
	    }
	}	
    }
    
    if($errorCount < 1)
    {
	print "No content errors\n";
    }
}


###########################################################################
# Makes syntax checks that the given name is according to specification
#
#
# Params: file name
#
###########################################################################
sub checkCentrepKeySheetName
{
    my $name = shift;
   
    # ----------------------------------------------------------------  
    # check name syntax
    # ----------------------------------------------------------------      
    if($name =~ /[A-Z]/)
    {
	print "Sheet: $name, no capital letters allowed\n";
    }
    if($name !~ /^[a-z0-9\_\.]+$/)
    {
	print "Sheet: $name, non valid characters in the name. (0-9 a-z _ and . are allowed\n";
    }
    if($name !~ /^keys_[a-z0-9\_]+\.xls$/)
    {
	print "Sheet: \"$name\", syntax should be \"keys_<modulename>.xls\"\n";
    }
	
}

1;




