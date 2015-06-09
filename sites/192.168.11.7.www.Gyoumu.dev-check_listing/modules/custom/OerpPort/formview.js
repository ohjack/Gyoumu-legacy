//Drupal.behaviors.FormViewController = function(){
$(document).ready(function(){
  $('form.oerp-formview').oerpformview();
  $('table.oerp-treeview').oerptreeview();
  
  $('[id="edit-func:submit"]').click(function(ev){
    $('form.oerp-formview:eq(0)').oerpformview('update');
    $('table.oerp-treeview').oerptreeview('update');
  });
});