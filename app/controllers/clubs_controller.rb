class ClubsController < ApplicationController
layout "users"
  before_action :set_club, only: [ :edit, :update, :destroy]

  # GET /clubs
  # GET /clubs.json
  def index
    @clubs = Club.all
  end

  # GET /clubs/1
  # GET /clubs/1.json
  def show
    #@club = Club.where(name: params[:param])
    @reviews = Review.where(club_id: @club.object_id)
    #.order("created_at DESC")

    if @reviews.blank?
      @avg_revew = 0
    else
      @avg_review = @reviews.average(:rating)
      #.round(2)
    end
  end

  # GET /clubs/new
  def new
    @club = Club.new
    #@club = current_user.clubs.build
  end

  # GET /clubs/1/edit
  def edit
  end
	
  # POST /clubs
  # POST /clubs.json
  def create
    @club = Club.new(club_params)
    #@club = current_user.clubs.build(club_params)

    respond_to do |format|
      if @club.save
        format.html { redirect_to @club, notice: 'Club was successfully created.' }
        format.json { render :show, status: :created, location: @club }
      else
        format.html { render :new }
        format.json { render json: @club.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /clubs/1
  # PATCH/PUT /clubs/1.json
  def update
    respond_to do |format|
      if @club.update(club_params)
        format.html { redirect_to @club, notice: 'Club was successfully updated.' }
        format.json { render :show, status: :ok, location: @club }
      else
        format.html { render :edit }
        format.json { render json: @club.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /clubs/1
  # DELETE /clubs/1.json
  def destroy
    @club.destroy
    respond_to do |format|
      format.html { redirect_to clubs_url, notice: 'Club was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.

    def set_club
      @club = Club.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def club_params
      params.require(:club).permit(:Name, :admin, :numberOfMembers, :category, :reviews, :rating)
    end
  end
