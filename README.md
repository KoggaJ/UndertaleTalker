**Undertale Talker** is a simple CMD program created for [Shadoenix](https://www.youtube.com/@Shadoenix) that gathers voice samples and glues them together into a single wav file, emulating the speaking sounds of Undertale and Deltarune characters.

## 1. How to install
Download the "[UndertaleTalker.exe](https://github.com/KoggaJ/UndertaleTalker/blob/main/UndertaleTalker.exe "UndertaleTalker.exe")" and put it in its own folder. Now, you will need to source the audio files from whichever game you wish to emulate.

For legal reasons, this repo has no copyrighted sounds, but you may use the "example_voice" which was made specifically for this project.

## 2. How to use
Since Undertale voices are made of multiple samples, each character voice has to be sorted into its own folder, for example

📁Undertale Talker Folder<br/>
├───⚙️UndertaleTalker.exe<br/>
├───📁Character_1<br/>
├──────🔊Voice_1.wav<br/>
└──────🔊Voice_2.wav<br/>
└───📁Character_2<br/>
├──────🔊Voice_1.wav<br/>
└──────🔊Voice_2.wav<br/>

Keep in mind, all the audio files have to be in **mono**, as well as **44100 sample rate**. if the audio is structured differently, the program might refuse to accept it or have otherwise unexpected behavior.

After setting up the EXE and the character folders, run the EXE file. You will be prompted to enter your text. This is where the character voice will go. Each letter in your input will be converted to a sound, so if you enter "I like toast", the audio will play once, then pause, then play 4 times, then pause again, before playing 5 times and generating the file. Spaces, comas and periods will be treated as pauses. You can stack multiple of them together for a longer pause.

Next input will ask you about voice folder. There you select exactly which folder contains the voice you want. If we select Character_2 (like in the example above), the audio samples will be taken from "Character_2" folder.

After submitting both inputs, the program will generate a wav file in the same place as its EXE.

![Sans undertale](https://static.wikia.nocookie.net/undertale/images/d/d1/Sans_overworld_tricycle.png/revision/latest?cb=20220122222621)
