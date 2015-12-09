# MooseEdit
## Character Trainer/Editor for Divinity: Original Sin (and Enhanced Edition)

### Developed by Dairymoose
Originally published on the Larian.com forums  
http://larian.com/forums/ubbthreads.php?ubb=showflat&Number=521295&page=29

#### Original Code Description by Dairymoose  
http://larian.com/forums/ubbthreads.php?ubb=showflat&Number=573801#Post573801
##### 573731 - 11/02/15 05:26 PM  Re: Save Game Editor [Re: Dairymoose]
> **Dairymoose**  
*member*  
*Registered: 07/15/14*  
*Posts: 102*  

>I don't really have time to address this right now but I can publish the source code.
I don't know how similar the save format is but I can't imagine it is all that different.
It is probably either a change to some of the file paths in the pak files, or they added a new data type.

##### 573801 - 11/03/15 12:35 AM  Re: Save Game Editor [Re: Dairymoose]  

> **Dairymoose**  
*member*  
*Registered: 07/15/14*  
*Posts: 10*  

> Well I did it. I put the source online.
To anyone who actually wants to try to make sense of it, I would suggest getting Qt Creator (IDE) and opening the MooseEdit.pro project file. That will greatly simplify getting it to work.

> Anyway here is some basic info:
* LsbReader/LsbWriter is for reading/writing lsb files.
* LsbObject is the in-memory representation of an lsb file's "tree node"
* SanityHash is for sanity.lsb generation
* PakCompressor handles PAK compression
* PakReader/PakWriter is for reading/writing PAK files
* CharacterLoader is for loading character data from the LSB and making the character tabs on the UI
* GamePakData has all the hardcoded filenames from the PAK files, you might have to modify this
* AppearanceEditorFrame is a gigantic file that has all of the openGL 3d rendering code
* mainwindow contains most of the UI functionality

> A lot of the code is unrefined and ugly and should probably be rewritten. But it should mostly be error free.

> https://www.mediafire.com/folder/xggb2364417f4/MooseEdit

> Source is under: Source/MooseEdit-src.zip

> and since I am open sourcing it here is the formula used to calculate weapon damage:
`double ItemLabel::calculateDamage(long damageStat, long itemLevel, long damageBoost) {  
return (itemLevel+1)*((50.0l*(exp(((damageStat+1)*0.1) - 0.1) - 1.067999958992004l) + 10))* 0.25 * (1 + damageBoost/100.0l);  
}`
