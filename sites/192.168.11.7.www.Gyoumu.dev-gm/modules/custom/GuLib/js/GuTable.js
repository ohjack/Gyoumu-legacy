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


  if(Drupal.tableDrag){
    var funcDragRow = Drupal.tableDrag.prototype.dragRow;
    
    Drupal.tableDrag.prototype.dragRow = function(event, self){
      funcDragRow.call(this, event, self);

      var selIndent = 'div.indentation';

      $(selIndent).each(function(){
        var Tr  = $(this).parents('tr:eq(0)')
        Tr.addClass('grouped');

        if(Tr.next('tr').find(selIndent).length < 1){
          Tr.addClass('tail');
        }

        var TrPrev = Tr.prev('tr');
        TrPrev.addClass('grouped').removeClass('tail');

        if(TrPrev.find(selIndent).length < 1){
          TrPrev.addClass('head');
        }
      });

      $('tr.grouped').each(function(){
        var Tr = $(this);

        if(Tr.hasClass('head')){
          if(Tr.next('tr').find(selIndent).length < 1){
            Tr.removeClass('grouped').removeClass('head');
          }
        }
        else{
          if(Tr.find(selIndent).length < 1){
            Tr.removeClass('grouped').removeClass('tail');

            var TrPrev = Tr.prev('tr');

            if(TrPrev.hasClass('head')){
              TrPrev.removeClass(('head')).remove('grouped');
            }
            else if(TrPrev.hasClass('grouped')){
              TrPrev.addClass('tail');
            }
          }
        }
      });
    }
  }
};