Documentation

Initial thoughts:

The task leaves a lot of questions open for interpretation:

What assets am I going to use for the VFX, SFX, Animations?
VFX: check unreal market place for free assets
SFX: Soundcloud? FreeSounds, UE Marketplace
Animations: Mixamo should do

I’m not a technical programmer, how will i handle animations?
Probably gonna copy some stuff from the lyra project, anim layers are cool anyway
This way i can  easily implement different animations for the different fire arms


Initial Plan:
Make full use of the gameplay ability system, because it heckin cool. I'll start with a plain ThirdPerson project though
since anything else would be overkill. And, after all, this is meant to show that I can work with the tool, not that I know
how to copy paste Lyra - though that would be easy. :)

1) Make a custom AbilitySystemComponent class with custom triggers that integrates EnhancedInput (C++)
2) Make a custom EquipmentComponent class that handles weapon equipping and unequipping
	-> Weapons give the player Gameplay Abilitites that allow the player input to interface the weapon functionality
3) C++ base class for firearms that allows modifiers
	-> the "Weapon" will be a data asset, data driven design is cool for asset creation
	-> Weapons will have proxy actors which are the in-game representation of the weapon, separating visualization from functionality
	-> Ammo should also be variable
	3.1) Make Ammo class that can be dynamically set on the weapon. The ammo should probably handle behaviour related to the projectile,
	like if its a hitscan, physics simulated or other projectile
4) Implement a simple Pistol in blueprint, make it shoot (probably a hitscan projectile)
	-> Trail VFX
	-> Fire SFX
	-> Impact SFX/VXF
	-> GameplayEvent for impact, we dont need "damage" yet since we just want to detect hits on moving targets
	-> Animations
5) The shooting range, make spawners that can be parametrized in data, and spawn the targets which move along a spline
	5.1) make the targets, simple concentric cylinders, define hit detection, and scoring
	5.2) A subsystem would be good for keeping track of the score, make that in C++
6) Implement some modifiers
7) Implement some ammo types
8) ????
9) Profit!

Optional:
10) Implement more weapons
	(Automatic rifle, Continuous Laser Rifle (why not), Semi automatic rifle

A description of the Weapons-Ammo-Modifiers system in text and UML format:

An Actor that wants to use weapons should use the UWeaponManagerComponent. First, a UDataAsset of type UWeaponData needs to be created. This data asset holds the meta information for what the Weapon is. Then a new Actor needs to be created as the weapons visual proxy. Then a UWeaponInstance blueprint needs to be created as the object that would handle runtime information (like ammo count, which I didnt implement for the time being). The Proxy and Instance class need to be referenced in the WeaponData. Furthermore, a UDataAsset of type UAbilitySet should be created and referenced to define what abilities, effects and attribute sets will be given to the player while this weapon is equipped. Also an Input Mapping Context to define weapon specific inputs.

For Modifiers its similar - create a UDataAsset of type UWeaponModifierData, a Blueprint of UModifierInstance and an Actor Proxy. The Instance defines what the modifier actually does - in case of the implemented examples, the silencer overrides the OnFire Gameplay Cue Tag, and the laser mod ... well it doesnt tick (yet) so the laser mod is done with the proxy (I just wanted to be done when i made that :C).

For Ammunition its the same, create a UDataAsset of type UWeaponAmmunitionData and a blueprint of UWeaponAmmunitionInstance. Define meta data in the data class, and the OnFire function in the instance. From here on its flexible, the supplied implementation is a simple hitscan projectile, but you could just as well spawn a physics simulated projectile.

The Ammunitions and Modifiers are then to be referenced in the Weapon Data Asset they are supposed to be linked in.

The Weapon Configuration Widget can be added to the screen by pressing Tab, and then you can click the relevant icons on screen to modify the weapon instance at runtime, according to the weapon data.


A description of any limitations or problems encountered:

One problem remains in the weapon config ui, where the list entries dont clear properly when selecting a different weapon, making mod buttons get duplicated. It's not game breaking and I didnt want to invest too much time in the HUD, so I left it as is. The system is very Lyra-esque. Lyra assets were used in the project, and the weapon definition structure is similar, but I did not copy it over. I just did it that way because i recently spent a whole lot of time working with Lyra, like the system and it makes sense. Again, no code has been copied from Lyra. But, in terms of limitations, this means the system is very extendable and pretty easy to maintain. Now that the structures have been defined, making new weapons is data driven, and code only needs to be modified when theres a bug I overlooked or when new hooks for gameplay need to be created. However, one downside of this current system is that weapon instance runtime data only exists while the weapon instance is managed by the WeaponManagerComponent. This might be an issue when, for example, wanting to throw the weapon on the ground, though I'm certain working around this limitation would be as easy as handing the instance over from the manager to the weapon thats gonna be on the ground.

Total time spent on the feature, with breakdowns for weapons, modifiers, ammo, targets, and hit registration if possible.
- Setting up Project Gameplay Ability stuff (including Inputs) - 3h
- Defining the basic structures for Weapons, Ammo and Modifiers - 2h
- Refining those structures to suit the project needs as i went along - probably like 4h 
(Weapons, Ammo and Mods were pretty even in terms of how complex they were to make, so id say outo f those 6h its 2h for each)
- Creating the WeaponManagercomponent with all its functionality - 3h
- Creating the Weapon Config UI - 4h
- Looking for assets before i gave up and just made my own low quality sound effects and particle systems - 1h