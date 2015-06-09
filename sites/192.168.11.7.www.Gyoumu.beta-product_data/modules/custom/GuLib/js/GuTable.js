Drupal.behaviors.GuTableController = function(){
  ShowTooltip = function(e)
  {
  	var $this = $(this);
  	var text = $this.next('span.show-tooltip-text');
  	var posi = $this.position();
  	
  	text.show()
  		.css('top', posi.top + $this.height() + 5)
  		.css('left', posi.left);
  
  	return false;
  }
  
  HideTooltip = function(e)
  {
    var $this = $(this);
  	var text = $this.next('span.show-tooltip-text');
  	text.hide();
  }

  $('table.gu-table span.show-tooltip-text')
    .prev('span')
    .hover(ShowTooltip, HideTooltip);

  var funcMarkChanged = Drupal.tableDrag.prototype.row.prototype.markChanged;
  Drupal.tableDrag.prototype.row.prototype.markChanged = function(){
    funcMarkChanged.call(this);
    var selIndent = 'div.indentation';

    $(selIndent).each(function(){
      var Tr  = $(this).parents('tr:eq(0)')
      Tr.addClass('highlight');

      var TrPrev = Tr.prev('tr');
      if(!TrPrev.hasClass('highlight')){
        Tr.prev('tr').addClass('ex-highlight')
      }
    });

    $('tr.highlight').each(function(){
      var Tr = $(this);
      if(Tr.find(selIndent).length == 0){
        Tr.removeClass('highlight');
        Tr.prev('tr').removeClass('ex-highlight');
      }
    });
  }
};