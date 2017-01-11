#!/bin/sh
# Simple script used to generate the files containing metadata from DICOM Series
# When executing the script, you must be in the DICOM foler containing the images
# Don't forget to add the file 'DICOMFILES' to the DICOM folder

# ===========================================
# Generate the file
# ===========================================
generateFile()
{
    echo "{"
    
    extractValue $1 "0020,000e" "SeriesInstanceUID"
    extractValue $1 "0008,0060" "Modality"
    extractValue $1 "0008,0021" "SeriesDate"
    extractValue $1 "0008,0031" "SeriesTime"
    extractValue $1 "0008,103e" "SeriesDescription"
    extractValue $1 "0008,1050" "PerformingPhysiciansName"
    extractValue $1 "0010,0020" "PatientID"
    extractValue $1 "0010,0010" "PatientName"
    extractValue $1 "0010,0030" "PatientBirthDate"
    extractValue $1 "0010,0040" "PatientSex"
    extractValue $1 "0020,000d" "StudyInstanceUID"
    extractValue $1 "0008,0020" "StudyDate"
    extractValue $1 "0008,0030" "StudyTime"
    extractValue $1 "0008,0090" "ReferringPhysicianName"
    extractValue $1 "0008,1030" "StudyDescription"
    extractValue $1 "0010,1010" "PatientAge"
    extractValue $1 "0008,0080" "InstitutionName"
    extractValue $1 "0028,0030" "PixelSpacing"
    extractValue $1 "0018,0050" "SliceThickness"
    extractValue $1 "0020,0032" "ImagePositionPatient"
    extractValue $1 "0028,0010" "Rows"
    extractValue $1 "0028,0011" "Columns"
    extractValue $1 "0028,1050" "WindowCenter"
    extractValue $1 "0028,1051" "WindowWidth"
    extractValue $1 "0028,0004" "PhotometricInterpretation"
    extractValue $1 "0028,0100" "BitsAllocated"
    


    echo "}"
}

# ===========================================
# Extract a value and print it in Json format
# ===========================================
extractValue()
{
    #name=$(dcmdump $1 --search "$2" -s | sed "s/.* //g");
    name=$3
    var=$(dcmdump $1 --search "$2" -s | cut -d" " -f 3- | cut -d"#" -f1 | sed "s/\\\/\\\\\\\/g" | sed "s/ *$//g" | sed "s/^=//" | sed "s/^\[//" | sed "s/\]$//");
    
    [ "$var" = "" ] && var="";     #The tag is not defined
    
    [ "$var" = "(no value available)" ] && var="";  #The value is not defined
    
    echo -n "\t\"$name\": \"$var\""
    
    # We don't add a comma for the last value
    if [ "$2" != "0028,0100" ]
    then
        echo ","
    else
        echo "\n"
    fi
}

# ===========================================
# Get a value
# ===========================================
getValue()
{
    var=$(dcmdump $1 --search "$2" -s | cut -d" " -f 3- | cut -d"#" -f1 | sed "s/\\\/\\\\\\\/g" | sed "s/ *$//g" | sed "s/^=//" | sed "s/^\[//" | sed "s/\]$//");
    
    [ "$var" = "" ] && var="";     #The tag is not defined
    
    [ "$var" = "(no value available)" ] && var="";  #The value is not defined
    
    echo "$var";
}


# ===========================================
# ===========================================
# ===========================================

if [ $# != 1 ]
then
    return 1
fi

cd $1   #You must specify the dicom folder
rm -rf META
mkdir META

for file in `cat DICOMFILES`
do
    folder=$(echo "$file" | sed "s/^.\///" | sed "s/\/im.*//g") # | sed "s/\//_/g")
    SeriesInstanceUID=$(getValue $file "0020,000e")
    echo -n "$folder/$SeriesInstanceUID ... "
    mkdir -p ./META/$folder
    generateFile $file >> "./META/$folder/$SeriesInstanceUID.json"
    sed "s/\\\/\\\\\\\/g" -i META/$folder/$SeriesInstanceUID.json  #escape backslashes
    echo "ok" 
done







