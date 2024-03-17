meshconv.exe mesh guy.glb -o ./
meshconv.exe anim guy.glb -o ./ -fps 12

move body_mesh.mesh ..\..\Game\run_tree\Data\Models\guy.mesh
move idle.anim ..\..\Game\run_tree\Data\Animations\guy_idle.anim
move jump.anim ..\..\Game\run_tree\Data\Animations\guy_jump.anim

