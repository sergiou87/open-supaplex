#!/bin/sh

usage() {
	echo "Converts to WAVE the original Supaplex SoundBlaster samples in BLASTER.SND"
	echo "Usage: $0 snd_file"
	exit 1
}

outdir=wav-blaster

# Recieves:
# * Offset (bytes) of the VOC file in the SND file
# * Lenght (bytes) of the VOC file
# * Sound name, used in the output filename
voc2wav() {
	# * Extract the .voc file (Creative Voice File) from the .snd file
	# * Export as 16-bit PCM WAVE file
	# * Reduce the volume just enough to avoid clipping during playback
	# * Upsample from 8333 to 44100 Hz
	dd if="${infile}" bs=1 skip=$1 count=$2 | \
		sox - \
		-b 16 -e signed-integer "${outdir}/$3.wav" \
		gain -3.6 \
		rate 44100
}

[ $# -ne 1 ] && usage

infile="$1"
if [ ! -f "${infile}" ] ; then
	echo "${infile} not found."
	exit 1
fi

[ -d "${outdir}" ] || mkdir "${outdir}"

voc2wav   655 11077 'explosion'
voc2wav 11732  3915 'infotron'
voc2wav 15647  1897 'push'
voc2wav 17544  2131 'fall'
voc2wav 19675  1983 'bug'
voc2wav 21658   256 'base' # the real length is 123, but sox does not like that
voc2wav 21781 15047 'exit'
