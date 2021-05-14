# mtb_fbx (c) by Mel Massadian
#
# mtb_fbx is licensed under a
# Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License.
#
# You should have received a copy of the license along with this
# work. If not, see <http://creativecommons.org/licenses/by-nc-nd/4.0/>.

# RM -rf _build
conan source . --source-folder=_build/source
conan install . --install-folder=_build/build/ --build=missing -s build_type=Release
conan build . --source-folder=_build/source --build-folder=_build/build
