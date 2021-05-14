# mtb_fbx (c) by Mel Massadian
#
# mtb_fbx is licensed under a
# Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License.
#
# You should have received a copy of the license along with this
# work. If not, see <http://creativecommons.org/licenses/by-nc-nd/4.0/>.

# RM -rf _build
set -e
pushd "/Users/imac/Daz 3D/Applications/DAZ 3D/DAZStudio4 64-bit/plugins/"
RM -rf "libdzunitybridge.dylib"
popd
pushd "_build/build/Unity/DazStudioPlugin/"
cp "libdzunitybridge.dylib" "/Users/imac/Daz 3D/Applications/DAZ 3D/DAZStudio4 64-bit/plugins/"

# "/Users/imac/Daz 3D/Applications/DAZ 3D/DAZStudio4 64-bit/plugins/libdzunrealbridge.dylib"
