class ReviewsController < ApplicationController
	layout "users"
  before_action :set_review, only: [:show, :edit, :update, :destroy]
  before_action :set_club

  # GET /reviews
  # GET /reviews.json

  # GET /reviews/1
  # GET /reviews/1.json
 
  # GET /reviews/new
  def new
    @review = Review.new
    #@club = @review.club
    # @club = Club.find_by_id(params[:club_id])
    # @review = @club.reviews.new
    #@club = Club.where(id: params[:param])
  end

  # GET /reviews/1/edit
  def edit
  end

  # POST /reviews
  # POST /reviews.json
  def create
    # @club = Club.find_by_id(params[:club_id])
    # @review = @club.reviews.new(review_params)
    #@club = Club.where(name: params[:club])
    #@review.club = params[:club]
    #@review = Review.new(review_params)
    #@club = Club.where(id: params[:param])
    #@review.club = Club.find_by_id(params[:club_id])

    @review = Review.new(review_params)
    @review.user_id = current_user.id
    @review.club_id = @club.id

    if @review.save
      redirect_to @club
    else
      render 'new'
    end
    # respond_to do |format|
    #   if @review.save
    #     format.html { redirect_to @club, notice: 'Review was successfully created.' }
    #     format.json { render :show, status: :created, location: @review }
    #   else
    #     format.html { render :new }
    #     format.json { render json: @review.errors, status: :unprocessable_entity }
    #   end
    # end
  end

  # PATCH/PUT /reviews/1
  # PATCH/PUT /reviews/1.json
  def update
    @review.update(review_params)
    # respond_to do |format|
    #   if @review.update(review_params)
    #     format.html { redirect_to @review, notice: 'Review was successfully updated.' }
    #     format.json { render :show, status: :ok, location: @review }
    #   else
    #     format.html { render :edit }
    #     format.json { render json: @review.errors, status: :unprocessable_entity }
    #   end
    # end
  end

  # DELETE /reviews/1
  # DELETE /reviews/1.json
  def destroy

    @review.destroy
    redirect_to @club
    # @review = Review.find(params[:id])
    # @review.destroy
    # respond_to do |format|
    #   format.html { redirect_to reviews_url, notice: 'Review was successfully destroyed.' }
    #   format.json { head :no_content }
    # end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_review
      @review = Review.find(params[:id])
    end

    def set_club
      @club = Club.find(params[:club_id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def review_params
      params.require(:review).permit(:rating, :title, :comment, :club_name, :user)
    end
end
