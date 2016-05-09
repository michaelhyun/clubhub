class CategoriesController < ApplicationController
layout "users"


  # GET /categories
  # GET /categories.json
  def index
    @categories = Category.all
  end

  # GET /categories/1
  # GET /categories/1.json
  def show

  if (params[:param].eql? "Academics")
    @categories = Category.where(genre: 'Academics')
  end

  if (params[:param].eql? "Engineering")
    @categories = Category.where(genre: 'Engineering')
  end

  if (params[:param].eql? "Cultural")
    @categories = Category.where(genre: 'Cultural')
  end

  if (params[:param].eql? "Religious")
    @categories = Category.where(genre: 'Religious')
  end

  if (params[:param].eql? "Science")
    @categories = Category.where(genre: 'Science')
  end

  if (params[:param].eql? "Other")
    @categories = Category.where(genre: 'Other')
  end

  if (params[:param].eql? "Service and Social Justice")
    @categories = Category.where(genre: 'Service and Social Justice')
  end

  if (params[:param].eql? "Performing Arts")
    @categories = Category.where(genre: 'Performing Arts')
  end

  if (params[:param].eql? "Greek")
    @categories = Category.where(genre: 'Greek')
  end

  if (params[:param].eql? "Arts")
    @categories = Category.where(genre: 'Arts')
  end

  if (params[:param].eql? "Health")
    @categories = Category.where(genre: 'Health')
  end

  if (params[:param].eql? "Music")
    @categories = Category.where(genre: 'Music')
  end

  if (params[:param].eql? "Politics")
    @categories = Category.where(genre: 'Politics')
  end

  if (params[:param].eql? "Sports")
    @categories = Category.where(genre: 'Sports')
  end

end



  # GET /categories/new


  # GET /categories/1/edit


  # POST /categories
  # POST /categories.json


  # PATCH/PUT /categories/1
  # PATCH/PUT /categories/1.json
  #def update
   # respond_to do |format|
    #  if @category.update(category_params)
     #   format.html { redirect_to @category, notice: 'Category was successfully updated.' }
      #  format.json { render :show, status: :ok, location: @category }
     # else
      #  format.html { render :edit }
      #  format.json { render json: @category.errors, status: :unprocessable_entity }
     # end
   # end
 # end

  # DELETE /categories/1
  # DELETE /categories/1.json
  #def destroy
   # @category.destroy
   # respond_to do |format|
   #   format.html { redirect_to categories_url, notice: 'Category was successfully destroyed.' }
   #   format.json { head :no_content }
   # end
 # end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_category
      @categories = Category.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def category_params
      params.require(:category).permit(:genre, :clubs)
    end
   end
