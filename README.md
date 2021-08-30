Go to https://streamelements.com/dashboard/account/channels
On the right there should be a button to "Show secrets". Click this.
There should be some extra text called "JWT Token". The JWT Token is effectively login credentials 
so do not share these with anybody.

make a note of the "Account ID" and "JWT Token"

=========

After installing the plugin there should be a new section called StreamElements. Under this section will be 'Store Item' and 'Close Store'

=========

'Store Item' is used to interact with a specific store item in a specific channel.

Title - Generally shouldn't be used and is replaced by Display Name

Display Name - This should be used instead of Title. If a store item has a restricted number of redemptions then the button will display how many are left under the display name.

Channel - This should be the ID of the channel you want to query. This is the "Account ID" captured previously.

Store Item - This is the exact name of the store item including capital letters. e.g. 2K Single Raffle

Amount - This is the amount of the item that should be made available when enabled. -1 is unlimited

JWT Token - This is the "JWT Token" captured previously. You only need to input this token once and click the Set button underneath. This token will then be saved within the stream deck and when the stream deck button loses focus within the app the text box will go blank.

------
Action

Pressing the button will toggle the status of the store item. If the item is disabled it will become enabled and if enabled will become disabled.

------
Button Status

Green - The button will be green if the store item is enabled. If the store item has a restricted number of redemptions then the amount left will also be displayed on the button.

Blue - This button will be blue if the store item is enabled and sold out.

Red - The button will be red if the store item is disabled.

Yellow - The button will be yellow if there has been an issue retrieving data for this item. This could be because the Channel or Store Item data is incorrect

=========

Close Store will close all enabled store items for the designated channel

Title - You can use the title on this button

Channel - This should be the ID of the channel you want to query. This is the "Account ID" captured previously.

This button will only work properly if you have atleast 1 'Store Item' button for the channel.

------
Action

Pressing the button will turn every store item, whether it is on the stream deck or not, to disabled.

------
Button Status

Green - The button will be green if there is an enabled store item that is also a button on the stream deck

Red - This button will be red if there are no enabled store items that are also a button on the stream deck. This does not mean there are no enabled store items just that the stream deck isn't monitoring them
