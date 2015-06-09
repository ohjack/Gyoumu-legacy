$(document).ready(function(){
  $('form.oerp-formview').oerpformview();
  $('table.oerp-treeview').oerptreeview();
  
  $('[id="edit-ns:Oerp:Page:FormView:Callee:Save"],' +
      '[id="edit-ns:Oerp:Page:TreeView:Callee:Save"]')
      .click(function(ev){

    $('form.oerp-formview:eq(0)').oerpformview('update');
  });

  $('div.toolbar-bottom input[type=submit]').click(function(ev){
    var $toolbar = $(this).parents('div.toolbar-bottom:eq(0)');
    var $treeview = $toolbar.prevAll('div.oerp-treeview:eq(0)');
    var sels = [];

    $treeview.find('tbody tr.selected').not('[merged]').each(function(){
      sels.push($(this).attr('oerp_rid'));
    });

    $('#edit-sels').val(JSON.stringify(sels));
  });
});