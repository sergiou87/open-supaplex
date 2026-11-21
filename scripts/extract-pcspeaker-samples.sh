#!/bin/sh

usage() {
	echo "Converts to WAVE the original Supaplex PC speaker samples in SAMPLE.SND"
	echo "Usage: $0 snd_file"
	exit 1
}

outdir=wav-sample

# Recieves:
# * Offset (bytes) of the sample file in the SND file
# * Lenght (bytes) of the sample file
# * Sound name, used in the output filename
raw2wav() {
	# * Extract the raw sample from the .snd file
	# * Export as 16-bit PCM WAVE file
	# * Remove the DC offset (center the waveform)
	# * Amplify it so that the percieved loudness is -7 LUFS, approximately
	#   the same as the SoundBlaster samples, counting the gain reduction
	#   applied to those.
	# * Upsample from 8333 to 44100 Hz
	dd if="${infile}" bs=1 skip=$1 count=$2 | \
		sox -t raw -e signed-integer -b 8 -r 8333 -c 1 - \
		-b 16 -e signed-integer "${outdir}/$3.wav" \
		dcshift -0.25 \
		gain 5 \
		rate 44100
}

[ $# -ne 1 ] && usage

infile="$1"
if [ ! -f "${infile}" ] ; then
	echo "${infile} not found."
	exit 1
fi

[ -d "${outdir}" ] || mkdir "${outdir}"

# The offsets and lenghts were calculated manually
raw2wav   376 10903 'explosion'
raw2wav 11408  3724 'infotron'
raw2wav 15361  1668 'push'
raw2wav 17144  1974 'fall'
raw2wav 19245  1838 'bug'
raw2wav 21195    87 'base'
raw2wav 21336 14773 'exit'
