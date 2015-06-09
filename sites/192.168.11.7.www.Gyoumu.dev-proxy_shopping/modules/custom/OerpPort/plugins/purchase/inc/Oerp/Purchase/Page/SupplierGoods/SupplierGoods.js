$(document).ready(function(){
  $('td input.ex-op').keyup(function(){
    var $input = $(this);
    var $tr = $input.parents('tr:eq(0)');

    $input.val() > 0
      ? $tr.addClass('selected')
      : $tr.removeClass('selected');
  });

  $('[id="edit-ns:Oerp:Purchase:Page:SupplierGoods:Callee:Create-Order"]').click(function(){
    var vals = {};

    $('tr.selected').each(function(){
      var $tr = $(this);
      var rid = $tr.attr('oerp_rid');
      vals[rid] = $tr.find('input.ex-op').val();
    });

    $('[id="edit-view-json"]').val(JSON.stringify(vals));
  });
});